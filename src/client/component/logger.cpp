#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "console.hpp"

#include <utils/hook.hpp>

namespace logger
{
	namespace
	{
		utils::hook::detour com_error_hook;

		void print_error(const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::error(buffer);
		}

		void print_com_error(int, const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::error(buffer);
		}

		void com_error_stub(const int error, const char* msg, ...)
		{
			char buffer[2048];

			{
				va_list ap;
				va_start(ap, msg);

				vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

				va_end(ap);

				console::error("Error: %s\n", buffer);
			}

			com_error_hook.invoke<void>(error, "%s", buffer);
		}

		void print_warning(const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::warn(buffer);
		}

		void print(const char* msg, ...)
		{
			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::info(buffer);
		}

		void print_dev(const char* msg, ...)
		{
			static auto* enabled =
				game::Dvar_RegisterBool("logger_dev", false, game::DVAR_ARCHIVE, "Print dev stuff");
			if (!enabled->current.enabled)
			{
				return;
			}

			char buffer[2048];

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

			va_end(ap);

			console::info(buffer);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			com_error_hook.create(game::Com_Error, com_error_stub);
		}
	};
}

REGISTER_COMPONENT(logger::component)
