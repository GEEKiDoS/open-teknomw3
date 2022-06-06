#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>

namespace filesystem
{
	namespace
	{
		const char *sys_default_install_path()
		{
			static char work_dir[MAX_PATH];

			if (!*work_dir) 
			{
				GetCurrentDirectoryA(MAX_PATH, work_dir);
			}

			return work_dir;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			utils::hook::jump(0x41D620, sys_default_install_path);
		}
	};
}

REGISTER_COMPONENT(filesystem::component)
