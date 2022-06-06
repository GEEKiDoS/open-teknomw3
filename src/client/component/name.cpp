
#include <std_include.hpp>
#include <utils/hook.hpp>

#include "game/game.hpp"
#include "loader/component_loader.hpp"

#include "component/scheduler.hpp"

namespace name
{
	namespace
	{
		const char* live_get_user_name()
		{
			static auto name = game::Dvar_FindVar("name");
			return name->current.string;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			utils::hook::set<uint32_t>(0x4E6705, game::DVAR_ARCHIVE);
			utils::hook::jump(0x431420, live_get_user_name);
		}
	};
}

REGISTER_COMPONENT(name::component)
