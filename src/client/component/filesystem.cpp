#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>

#include "game/game.hpp"
#include "component/dvar.hpp"

#include "filesystem.hpp"

namespace filesystem
{
	namespace
	{
		const char* sys_default_install_path()
		{
			static char work_dir[MAX_PATH];

			if (!*work_dir)
			{
				GetCurrentDirectoryA(MAX_PATH, work_dir);
			}

			return work_dir;
		}
	}

	file::file(std::string name)
		: name_(std::move(name))
	{
		char* buffer{};
		const auto size = game::FS_ReadFile(this->name_.data(), &buffer);

		if (size >= 0 && buffer)
		{
			this->valid_ = true;
			this->buffer_.append(buffer, size);
			game::FS_FreeFile(buffer);
		}
	}

	bool file::exists() const
	{
		return this->valid_;
	}

	const std::string& file::get_buffer() const
	{
		return this->buffer_;
	}

	const std::string& file::get_name() const
	{
		return this->name_;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			utils::hook::jump(0x41D620, sys_default_install_path);
			dvar::override_dvar_default("fs_basegame", "userraw");
		}
	};
}

REGISTER_COMPONENT(filesystem::component)
