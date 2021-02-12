#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace chr
		{
			class c_misc : public s<c_misc>
			{
			private:
				void*		wallhack;

				int			last_wallhack = 0;
			private:
				uint32_t	main_actor = 0; 
			private:
				int			get_wallhack();
			private:
				void		no_coll();
			public:
				void		work();
				void		setup();
			};
		}
	}
}