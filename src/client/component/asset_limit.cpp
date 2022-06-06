#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <game/game.hpp>

namespace asset_limit
{
	namespace
	{
		void reallocate_asset_pool(game::XAssetType const type, int const size)
		{
			auto entry_size = reinterpret_cast<int(*)()>(game::DB_GetXAssetSizeHandlers[type])();
			game::DB_XAssetPool[type] = utils::memory::allocate(entry_size * size);
			game::g_poolSize[type] = size; utils::memory::allocate(entry_size * size);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			reallocate_asset_pool(game::attachment, 128);
		}
	};
}

REGISTER_COMPONENT(asset_limit::component)
