#include "reducer.h"

void sdk::game::c_reducer::setup()
{
	this->toggle = sdk::util::c_config::Instance().get_var(XorStr("reducer"), XorStr("toggle"));
}

void sdk::game::c_reducer::work()
{
	if (!this->get_toggle()) return;
	sdk::util::c_log::Instance().duo(XorStr("[ f_SetFrameSkip %04x ]\n"), sdk::game::func::c_funcs::Instance().f_SetFrameSkip);
	sdk::game::func::c_funcs::Instance().f_SetFrameSkip(true);
}

int sdk::game::c_reducer::get_toggle()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->toggle);
	return std::stoi(r->container.c_str());
}