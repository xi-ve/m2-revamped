#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace chr
		{
			class c_pull : public s<c_pull>
			{
			private:
				ULONGLONG	last_pull = 0;
			private:
				void*		enable, * packet_method;
			private:
				int			get_enabled();
				int			get_packet_method();
			private:
				void		packet_based();
			public:
				void		setup();
				void		work();
			};
		}
	}
}