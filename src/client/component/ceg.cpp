#include <std_include.hpp>
#include <loader/component_loader.hpp>
#include <utils/hook.hpp>

#include "game/game.hpp"

namespace ceg
{
	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			// Generic killer caller.
			size_t killer_locs[] = {
				0x40B4B0, 0x41AA50, 0x42B680, 0x4353C0, 0x4CD7F0, 0x4D1170,
				0x4ED230, 0x4EFFF0, 0x4F0F60, 0x516E90,
			};
			
			for (auto loc : killer_locs)
				utils::hook::set<DWORD>(loc, 0xC301B0);
		}
	};
}

REGISTER_COMPONENT(ceg::component)
