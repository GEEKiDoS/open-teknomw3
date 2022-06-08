#include <std_include.hpp>

#include <utils/compression.hpp>
#include <utils/hook.hpp>
#include <utils/memory.hpp>

#include <game/game.hpp>

#include <component/scheduler.hpp>
#include <component/filesystem.hpp>

#include <xsk/gsc/types.hpp>
#include <xsk/gsc/interfaces/compiler.hpp>
#include <xsk/gsc/interfaces/assembler.hpp>
#include <xsk/resolver.hpp>
#include <gsc_interface.hpp>

#include "loader/component_loader.hpp"

namespace scripting::gsc
{
	namespace
	{
		auto compiler = ::gsc::compiler();
		auto assembler = ::gsc::assembler();

		game::ScriptFile* alloc_scriptfile()
		{
			auto buf = game::DB_AllocStreamPos(0);
			game::DB_IncStreamPos(sizeof(game::ScriptFile));

			return reinterpret_cast<game::ScriptFile*>(buf);
		}

		char* alloc_buffer(void* const original, size_t len)
		{
			auto buf = game::DB_AllocStreamPos(0);
			game::DB_IncStreamPos(len);

			std::memcpy(buf, original, len);

			return buf;
		}

		game::ScriptFile* load_custom_script(const char* name, std::string const &real_name)
		{
			filesystem::file source{ real_name + ".gsc" };
			if(!source.exists()) return nullptr;

			auto buffer = source.get_buffer();
			auto data = std::vector<uint8_t>{ buffer.begin(), buffer.end() };

			try 
			{
				compiler->compile(real_name, data);
			} 
			catch(std::exception& e)
			{
				printf(e.what());
				game::Com_Error(game::ERR_DROP, "Compile script %s failed!\nDetail:\n%s", real_name.data(), e.what());
				return nullptr;
			}

			auto assembly = compiler->output();

			assembler->assemble(real_name, assembly);

			auto scriptfile = alloc_scriptfile();
			scriptfile->name = name;

			auto bytecode = assembler->output_script();
			scriptfile->bytecode = alloc_buffer(bytecode.data(), bytecode.size());
			scriptfile->bytecodeLen = bytecode.size();

			auto stack = assembler->output_stack();
			auto stack_data = std::string{ reinterpret_cast<char *>(stack.data() + 4), stack.size() - 4 };
			auto compressed = utils::compression::zlib::compress(stack_data);

			scriptfile->len = stack.size() - 4;
			scriptfile->compressedLen = compressed.size();
			scriptfile->buffer = alloc_buffer(compressed.data(), compressed.size());

			return scriptfile;
		}

		game::ScriptFile* load_script(int, const char *name, int)
		{
			std::string real_name{ name };
			auto id = static_cast<uint16_t>(std::atoi(name));
			if (id) real_name = xsk::gsc::iw5::resolver::file_name(id);

			printf("Loading script %s (%s)\n", real_name.data(), name);

			auto* script = load_custom_script(name, real_name);
			if (script)
			{
				return script;
			}

			auto result = game::DB_FindXAssetHeader(game::scriptfile, name, true).scriptfile;

			printf("mempos: %p, rel address: %p\n", result->bytecode, result->bytecode - *game::programBuffer);

			return result;
		}

		void script_error(int, const char* format, ...)
		{
			va_list va;
			va_start(va, format);

			vprintf(format, va);

			va_end(va);
			game::Com_Error(game::ERR_DROP, format);
		}

		uint16_t resolve_token(std::string const &token)
		{
			return xsk::gsc::iw5::resolver::token_id(token);
		}

		std::vector<int> script_handles;

		void read_custom_scripts()
		{
			script_handles.clear();

			if (game::Scr_LoadScript("scripts/mapspawn"))
			{
				// 为什么不能用？？？？？？？？？？？？？？？？？？？？？？？？
				auto handle = game::Scr_GetFunctionHandle("scripts/mapspawn", resolve_token("init"));
				if (handle) script_handles.push_back(handle);
			}

			return game::GScr_LoadGameTypeScript();
		}

		void exec_custom_scripts()
		{
			for (auto handle : script_handles)
			{
				game::Scr_FreeThread(game::Scr_ExecThread(handle, 0));
			}

			return game::Scr_LoadGameType();
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			utils::hook::call(0x4F591E, load_script);
			utils::hook::jump(0x479270, script_error);

			utils::hook::call(0x4D0719, read_custom_scripts);
			utils::hook::call(0x52FFE8, exec_custom_scripts);
		}
	};
}

REGISTER_COMPONENT(scripting::gsc::component)
