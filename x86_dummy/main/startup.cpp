#include "startup.h"
void main::s_startup::setup()
{
	this->console();
	sdk::util::c_log::Instance().duo("[ starting memory init ... ]\n");
}
void main::s_startup::console()
{
	AllocConsole();
	auto a = freopen("CONIN$", "r", stdin);
	auto b = freopen("CONOUT$", "w", stdout);
	auto c = freopen("CONOUT$", "w", stderr);
	SetConsoleTitleA("");
}
