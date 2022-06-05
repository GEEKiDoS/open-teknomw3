#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "command.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>
#include <utils/io.hpp>

namespace command
{
	namespace
	{
		utils::hook::detour client_command_hook;

		std::unordered_map<std::string, std::function<void(params&)>> handlers;
		std::unordered_map<std::string, std::function<void(game::gentity_s*, params_sv&)>> handlers_sv;

		void main_handler()
		{
			params params = {};

			const auto command = utils::string::to_lower(params[0]);
			if (handlers.find(command) != handlers.end())
			{
				handlers[command](params);
			}
		}

		void client_command(const char client_num)
		{
			if (game::g_entities[client_num].client == nullptr)
			{
				// Client is not fully connected
				return;
			}

			params_sv params = {};

			const auto command = utils::string::to_lower(params[0]);
			if (const auto got = handlers_sv.find(command); got != handlers_sv.end())
			{
				got->second(&game::g_entities[client_num], params);
			}

			client_command_hook.invoke<void>(client_num);
		}

		// Shamelessly stolen from Quake3
		// https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/qcommon/common.c#L364
		void parse_command_line()
		{
			static auto parsed = false;
			if (parsed)
			{
				return;
			}

			static std::string comand_line_buffer = GetCommandLineA();
			auto* command_line = comand_line_buffer.data();

			auto& com_num_console_lines = *game::com_num_console_lines;
			auto* com_console_lines = game::com_console_lines.get();

			auto inq = false;
			com_console_lines[0] = command_line;
			com_num_console_lines = 0;

			while (*command_line)
			{
				if (*command_line == '"')
				{
					inq = !inq;
				}
				// look for a + separating character
				// if commandLine came from a file, we might have real line seperators
				if ((*command_line == '+' && !inq) || *command_line == '\n' || *command_line == '\r')
				{
					if (com_num_console_lines == 0x20) // MAX_CONSOLE_LINES
					{
						break;
					}
					com_console_lines[com_num_console_lines] = command_line + 1;
					com_num_console_lines++;
					*command_line = '\0';
				}
				command_line++;
			}
			parsed = true;
		}

		void parse_commandline_stub()
		{
			parse_command_line();
			reinterpret_cast<void(*)()>(0x1403CEE10)();
		}
	}

	void read_startup_variable(const std::string& dvar)
	{
		// parse the commandline if it's not parsed
		parse_command_line();

		auto& com_num_console_lines = *reinterpret_cast<int*>(0x147B76504);
		auto* com_console_lines = reinterpret_cast<char**>(0x147B76510);

		for (int i = 0; i < com_num_console_lines; i++)
		{
			game::Cmd_TokenizeString(com_console_lines[i]);

			// only +set dvar value
			if (game::Cmd_Argc() >= 3 && game::Cmd_Argv(0) == "set"s && game::Cmd_Argv(1) == dvar)
			{
				game::Dvar_SetCommand(game::Cmd_Argv(1), game::Cmd_Argv(2));
			}

			game::Cmd_EndTokenizeString();
		}
	}

	params::params()
		: nesting_(game::cmd_args->nesting)
	{
	}

	int params::size() const
	{
		return game::cmd_args->argc[this->nesting_];
	}

	const char* params::get(const int index) const
	{
		if (index >= this->size())
		{
			return "";
		}

		return game::cmd_args->argv[this->nesting_][index];
	}

	std::string params::join(const int index) const
	{
		std::string result = {};

		for (auto i = index; i < this->size(); i++)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}
		return result;
	}

	params_sv::params_sv()
		: nesting_(game::sv_cmd_args->nesting)
	{
	}

	int params_sv::size() const
	{
		return game::sv_cmd_args->argc[this->nesting_];
	}

	const char* params_sv::get(const int index) const
	{
		if (index >= this->size())
		{
			return "";
		}

		return game::sv_cmd_args->argv[this->nesting_][index];
	}

	std::string params_sv::join(const int index) const
	{
		std::string result = {};

		for (auto i = index; i < this->size(); i++)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}
		return result;
	}

	void add_raw(const char* name, void (*callback)())
	{
		game::Cmd_AddCommandInternal(name, callback, utils::memory::get_allocator()->allocate<game::cmd_function_s>());
	}

	void add(const char* name, const std::function<void(const params&)>& callback)
	{
		const auto command = utils::string::to_lower(name);

		if (handlers.find(command) == handlers.end())
			add_raw(name, main_handler);

		handlers[command] = callback;
	}

	void add(const char* name, const std::function<void()>& callback)
	{
		add(name, [callback](const params&)
		{
			callback();
		});
	}

	void add_sv(const char* name, std::function<void(game::gentity_s*, const params_sv&)> callback)
	{
		// doing this so the sv command would show up in the console
		add_raw(name, nullptr);

		const auto command = utils::string::to_lower(name);

		if (handlers_sv.find(command) == handlers_sv.end())
			handlers_sv[command] = std::move(callback);
	}

	bool cheats_ok(const game::gentity_s* ent)
	{
		if (!dvars::sv_cheats->current.enabled)
		{
			game::SV_GameSendServerCommand(ent->s.number, game::SV_CMD_RELIABLE,
				"f \"Cheats are not enabled on this server\"");
			return false;
		}

		if (ent->health < 1)
		{
			game::SV_GameSendServerCommand(ent->s.number, game::SV_CMD_RELIABLE,
				"f \"You must be alive to use this command\"");
			return false;
		}

		return true;
	}

	void execute(std::string command, const bool sync)
	{
		command += "\n";

		if (sync)
		{
			game::Cmd_ExecuteSingleCommand(0, 0, command.data());
		}
		else
		{
			game::Cbuf_AddText(0, command.data());
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			add_commands_generic();
		}

	private:
		static void add_commands_generic()
		{
			add("quit", game::Com_Quit_f);
			add("quit_hard", utils::nt::raise_hard_exception);
			add("crash", []
			{
				*reinterpret_cast<int*>(1) = 0x12345678;
			});

			add("vstr", [](const params& params)
			{
				if (params.size() < 2)
				{
					console::info("vstr <variablename> : execute a variable command\n");
					return;
				}

				const auto* dvarName = params.get(1);
				const auto* dvar = game::Dvar_FindVar(dvarName);

				if (dvar == nullptr)
				{
					console::info("%s doesn't exist\n", dvarName);
					return;
				}

				if (dvar->type != game::dvar_type::DVAR_TYPE_STRING
					&& dvar->type != game::dvar_type::DVAR_TYPE_ENUM)
				{
					console::info("%s is not a string-based dvar\n", dvar->name);
					return;
				}

				execute(dvar->current.string);
			});
		}
	};
}

REGISTER_COMPONENT(command::component)
