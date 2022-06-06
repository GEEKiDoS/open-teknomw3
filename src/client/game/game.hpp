#pragma once

#include "structs.hpp"

#define SELECT_VALUE(mp, dedi) (game::environment::is_dedi() ? (dedi) : (mp))

namespace game
{
	namespace environment
	{
		launcher::mode get_mode();

		bool is_mp();
		bool is_dedi();

		void set_mode(launcher::mode mode);

		std::string get_string();
	}

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t mp_address, const size_t dedi_address)
			: mp_obj_(reinterpret_cast<T*>(mp_address))
			  , dedi_obj_(reinterpret_cast<T*>(dedi_address))
		{
		}

		T* get() const
		{
			if (environment::is_dedi())
				return dedi_obj_;

			return mp_obj_;
		}

		operator T*() const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

		T& operator[](int index) const
		{
			return *reinterpret_cast<T*>(reinterpret_cast<size_t>(this->get()) + sizeof(size_t) * index);
		}

	private:
		T* mp_obj_;
		T* dedi_obj_;
	};

	int Cmd_Argc();
	const char* Cmd_Argv(int index);

	int SV_Cmd_Argc();
	const char* SV_Cmd_Argv(int index);
}

#include "symbols.hpp"
