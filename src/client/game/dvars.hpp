#pragma once

#include "structs.hpp"

namespace dvars
{
	extern game::dvar_t* sv_cheats;

	std::string dvar_get_vector_domain(const int components, const game::DvarLimits& domain);
	std::string dvar_get_domain(const game::dvar_type type, const game::DvarLimits& domain);
}
