#include "pickup.h"

void sdk::game::c_pickup::setup()
{
	this->toggle = sdk::util::c_config::Instance().get_var(XorStr("pickup"), XorStr("toggle"));
	this->distance = sdk::util::c_config::Instance().get_var(XorStr("pickup"), XorStr("range"));
	this->delay = sdk::util::c_config::Instance().get_var(XorStr("pickup"), XorStr("delay"));
}

void sdk::game::c_pickup::work()
{
	if (!this->get_toggle()) return;
	if (!sdk::game::item::c_item_gather::Instance().items.size()) return;

	if (GetTickCount64() > this->last_loot) this->last_loot = GetTickCount64() + this->get_delay();
	else return;
	
	auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!main_actor) return;

	auto main_pos = sdk::game::chr::c_char::Instance().get_pos(main_actor);
	if (!main_pos.valid()) return;

	auto closest_vid = 0; auto last_distance = 66666.f; auto last_pos = sdk::game::vec();
	auto cpy_list = sdk::game::item::c_item_gather::Instance().items;
	for (auto a : cpy_list)
	{
		auto dst_to_item = sdk::game::chr::c_char::Instance().get_distance(a.pos, main_pos);
		if (dst_to_item > this->get_distance()) continue;
		if (dst_to_item < last_distance)
		{
			last_distance = dst_to_item;
			closest_vid = a.vid;
			last_pos = a.pos;
		}
	}

	if (!closest_vid) return;

	auto network_manager = sdk::game::c_utils::Instance().baseclass_networking();
	if (!network_manager) return;

	if (last_distance > 300) sdk::game::c_waithack::Instance().interpolate_to_pos(main_pos, last_pos);
	sdk::game::func::c_funcs::Instance().f_SendItemPickUpPacket(network_manager, closest_vid);
	for (auto a : cpy_list)
	{
		if (a.vid == closest_vid) continue;
		auto dst_to_item = sdk::game::chr::c_char::Instance().get_distance(a.pos, last_pos);
		if (dst_to_item < 300) sdk::game::func::c_funcs::Instance().f_SendItemPickUpPacket(network_manager, a.vid);		
	}
	if (last_distance > 300) sdk::game::c_waithack::Instance().interpolate_to_pos(last_pos, main_pos);
}

int sdk::game::c_pickup::get_toggle()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->toggle);
	return std::stoi(r->container);
}

int sdk::game::c_pickup::get_distance()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->distance);
	return std::stoi(r->container);
}

int sdk::game::c_pickup::get_delay()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->delay);
	return std::stoi(r->container);
}
