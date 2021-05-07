#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace hooks
		{
			typedef bool(__fastcall* t_CPythonApplicationProcess)(uint32_t);
			extern t_CPythonApplicationProcess o_CPythonApplicationProcess;
			extern bool __fastcall f_CPythonApplicationProcess(uint32_t base);
		}
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