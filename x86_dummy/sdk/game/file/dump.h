#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace file
		{
			namespace dump_args
			{
				extern void*	filename;
				extern void*	data;
				extern void*	mapped;
				extern void*	base;
				extern uint32_t	return_val;
				extern sdk::game::func::t::t_EterPackGet o_EterPackGet;				
			}
			extern void rmv_str(std::string& clean_file_name, char c);
			extern void gate();
			class c_dump : public s<c_dump>
			{
			public:
				int		file_index = 0;
			public:
				void	set_ctx(void* f, void* d, void* m, void* b);
			private:
				bool	dump();
			public:
				void	work();
				void	setup();
			};
		}
	}
}