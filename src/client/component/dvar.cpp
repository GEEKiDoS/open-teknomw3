#include <std_include.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>

#include "game/game.hpp"
#include "loader/component_loader.hpp"

#include "component/scheduler.hpp"
#include "component/dvar.hpp"

namespace dvar
{
	game::dvar_t* sv_cheats = nullptr;

	namespace
	{
		std::unordered_map<std::string, uint32_t> dvar_flagmods;
		std::unordered_map<std::string, game::DvarValue> dvar_valuemods;

		utils::hook::detour dvar_register_bool_detour;
		game::dvar_t *dvar_register_bool_stub(const char* dvarName, bool value, uint32_t flags, const char* description)
		{
			if (dvar_flagmods.contains(dvarName))
			{
				flags = dvar_flagmods[dvarName];
			}

			if (dvar_valuemods.contains(dvarName))
			{
				value = dvar_valuemods[dvarName].enabled;
			}

			return dvar_register_bool_detour.invoke<game::dvar_t *>(dvarName, value, flags, description);
		}

		utils::hook::detour dvar_register_float_detour;
		game::dvar_t* dvar_register_float_stub(const char* dvarName, float value, float min, float max, unsigned int flags, const char* description)
		{
			if (dvar_flagmods.contains(dvarName))
			{
				flags = dvar_flagmods[dvarName];
			}

			if (dvar_valuemods.contains(dvarName))
			{
				value = dvar_valuemods[dvarName].enabled;
			}

			return dvar_register_float_detour.invoke<game::dvar_t*>(dvarName, value, min, max, flags, description);
		}
	}

	void override_dvar_flag(const std::string& dvar, uint32_t flags)
	{
		dvar_flagmods.emplace(dvar, flags);
	}

	void override_dvar_default(const std::string& dvar, game::DvarValue value)
	{
		dvar_valuemods.emplace(dvar, value);
	}

	bool cheats_ok(const char* cmd_name)
	{
		if (sv_cheats->current.enabled)
			return true;

		printf("%s is cheat protected.\n", cmd_name);
		return false;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			dvar_register_bool_detour.create(game::Dvar_RegisterBool.get(), dvar_register_bool_stub);
			dvar_register_float_detour.create(game::Dvar_RegisterFloat.get(), dvar_register_float_stub);

			override_dvar_flag("cg_fov", game::dvar_flags::DVAR_ARCHIVE);

			scheduler::once([]() {
				sv_cheats = game::Dvar_RegisterBool("sv_cheats", false, game::dvar_flags::DVAR_REPLICATED, "Enable cheats on the server");
			}, scheduler::main);
		}
	};
}

REGISTER_COMPONENT(dvar::component)
