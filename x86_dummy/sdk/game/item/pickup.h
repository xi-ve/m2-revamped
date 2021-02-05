#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		class c_pickup : public s<c_pickup>
		{
		private:
			ULONGLONG	last_loot = 0;
			void*		toggle, *distance, *delay;
		private:
			int			get_toggle();
			int			get_distance();
			int			get_delay();
		public:
			void		setup();
			void		work();
		};
	}
}