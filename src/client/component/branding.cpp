#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace branding
{

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
		}
	};
}

REGISTER_COMPONENT(branding::component)
