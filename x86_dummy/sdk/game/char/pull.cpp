#include "pull.h"

void sdk::game::chr::c_pull::setup()
{
	this->enable = sdk::util::c_config::Instance().get_var(XorStr("pull"), XorStr("toggle"));
	this->packet_method = sdk::util::c_config::Instance().get_var(XorStr("pull"), XorStr("packet"));
}

void sdk::game::chr::c_pull::work()
{
	if (!this->get_enabled()) return;
	if (this->last_pull > GetTickCount64()) return;
	this->last_pull = GetTickCount64() + 4000;
	if (this->get_packet_method()) this->packet_based();
}

int sdk::game::chr::c_pull::get_enabled()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->enable);
	return std::stoi(r->container.c_str());
}

int sdk::game::chr::c_pull::get_packet_method()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->packet_method);
	return std::stoi(r->container.c_str());
}

void sdk::game::chr::c_pull::packet_based()
{
	auto mobs = sdk::game::chr::c_char::Instance().get_alive();
	if (mobs.empty()) return;
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
	if (!network_base) return;
	auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!main_actor) return;
	auto main_pos = sdk::game::chr::c_char::Instance().get_pos(main_actor);
	for (auto a : mobs)
	{
		if (!(uintptr_t)a.second || (uint32_t)a.second == main_actor) continue;
		if (sdk::game::chr::c_char::Instance().get_type((uint32_t)a.second) == 0)
		{
			auto t_pos = sdk::game::chr::c_char::Instance().get_pos((uint32_t)a.second);
			if (sdk::game::chr::c_char::Instance().get_distance(t_pos, main_pos) > 8000) continue;
			if (sdk::game::func::c_funcs::Instance().f_SendAttackPacket)
			{
				sdk::game::func::c_funcs::Instance().f_SendAttackPacket(network_base, 0, a.first);
			}
			else
			{
				auto event_base = sdk::game::c_utils::Instance().baseclass_event_handler();
				auto mob_graph = sdk::game::chr::c_char::Instance().get_graphic_thing((uintptr_t)a.second);
				sdk::game::func::c_funcs::Instance().f_OnHit(event_base, 0, mob_graph, TRUE);
			}
		}
	}
}
