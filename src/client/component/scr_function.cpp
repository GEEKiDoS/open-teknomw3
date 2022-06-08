#include <std_include.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>

#include "game/game.hpp"
#include "loader/component_loader.hpp"

#include "component/scheduler.hpp"

#include <xsk/gsc/types.hpp>
#include <xsk/resolver.hpp>

namespace scr_function
{
	namespace
	{
		// fallback null function
		void scr_nullfunc() {}

		void* scr_find_function_with_check(int name)
		{
			auto func = game::g_scr_func_table[name];

			if (!func) {
				auto msg = utils::string::va("script function %d not exists!", name + 1);

#ifdef _DEBUG
				puts(msg);
				return scr_nullfunc;
#else
				throw std::runtime_error{ msg };
#endif
			}

			return func;
		}

		void __declspec(naked) scr_find_function_stub()
		{
			__asm 
			{
				mov dword ptr ds : [0x20B025C] , esi
				mov dword ptr ds : [0x20B0290] , edx

				push eax
				call scr_find_function_with_check
				add esp, 4

				call eax

				push 0x622AC9
				retn
			}
		}

		void scrcmd_precache_sound()
		{

		}
	}

	void add_function(std::string const& name, uint16_t id, void* function)
	{
		xsk::gsc::iw5::resolver::add_function(name, id);
		game::Scr_RegisterFunction(function, 0, id);
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			utils::hook::jump(0x6227CB, scr_find_function_stub);

			scheduler::once([]() {
				game::Scr_RegisterFunction(scrcmd_precache_sound, 0, 0x1C8);
			}, scheduler::pipeline::server);
		}
	};
}

REGISTER_COMPONENT(scr_function::component)
