#pragma once
#include <inc.h>
namespace main
{
	class s_startup : public s<s_startup>
	{
	private:
	public:
		void setup();
		void console();
	};
}