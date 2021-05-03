#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		class c_reducer : public s<c_reducer>
		{
		private:
			void *toggle;
		public:
			int get_toggle();
			void work();
			void setup();
		};
	}
}