#include "reducer.h"

void sdk::game::c_reducer::setup()
{
	sdk::util::c_log::Instance().duo(XorStr("[ o_CPythonApplicationProcess %04x ]\n"), sdk::game::func::c_funcs::Instance().o_CPythonApplicationProcess);
	this->toggle = sdk::util::c_config::Instance().get_var(XorStr("reducer"), XorStr("toggle"));
}

void sdk::game::c_reducer::work()
{
	if (!this->get_toggle()) return;
	MH_CreateHook((void*)sdk::game::func::c_funcs::Instance().o_CPythonApplicationProcess, (void*)sdk::game::hooks::f_CPythonApplicationProcess, (void**)&sdk::game::hooks::o_CPythonApplicationProcess);
	MH_EnableHook((void*)sdk::game::func::c_funcs::Instance().o_CPythonApplicationProcess);
}

int sdk::game::c_reducer::get_toggle()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->toggle);
	return std::stoi(r->container.c_str());
}

decltype(sdk::game::hooks::o_CPythonApplicationProcess) sdk::game::hooks::o_CPythonApplicationProcess = 0;
bool __fastcall sdk::game::hooks::f_CPythonApplicationProcess(uint32_t base)
{
	if (sdk::game::c_reducer::Instance().get_toggle()) 
		return true;
	
	return sdk::game::hooks::o_CPythonApplicationProcess(base);
}
