#include "startup.h"
void main::s_startup::setup()
{
	sdk::util::c_log::Instance().setup();
	this->console();
	sdk::util::c_log::Instance().duo("[ startup executed ]\n");
}
void main::s_startup::console()
{
	AllocConsole();
	auto a = freopen("CONIN$", "r", stdin);
	auto b = freopen("CONOUT$", "w", stdout);
	auto c = freopen("CONOUT$", "w", stderr);
	SetConsoleTitleA("");
}
