#pragma once

namespace dvar
{
	void override_dvar_flag(const std::string& dvar, uint32_t flags);
	void override_dvar_default(const std::string& dvar, game::DvarValue value);

	template<typename T>
	void override_dvar_default(const std::string& dvar, T value)
	{
		return override_dvar_default(dvar, *reinterpret_cast<game::DvarValue*>(&value));
	}

	bool cheats_ok(const char* cmd_name);
}
