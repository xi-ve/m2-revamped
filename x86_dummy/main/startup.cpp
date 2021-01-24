#include "startup.h"
void main::s_startup::setup()
{
	this->console();
	sdk::util::c_log::Instance().duo(XorStr("[ starting memory init ... ]\n"));
}
void main::s_startup::console()
{
	AllocConsole();
	auto a = freopen(XorStr("CONIN$"), XorStr("r"), stdin);
	auto b = freopen(XorStr("CONOUT$"), XorStr("w"), stdout);
	auto c = freopen(XorStr("CONOUT$"), XorStr("w"), stderr);
	SetConsoleTitleA("");
}
