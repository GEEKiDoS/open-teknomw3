#include <std_include.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>

#include "game/game.hpp"
#include "loader/component_loader.hpp"

namespace dlc
{
	namespace
	{
		bool check_dlc_license_vaild(int license)
		{
			return true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			utils::hook::jump(0x4CC4B0, check_dlc_license_vaild);
		}
	};
}

REGISTER_COMPONENT(dlc::component)
