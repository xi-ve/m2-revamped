#include "pickup.h"

void sdk::game::c_pickup::make_clean_conf()
{
	std::ofstream f(this->item_conf_file);
	for (auto a : sdk::game::item::c_item_manager::Instance().item_map)
	{
		this->item_conf.item_whitelist.push_back(a.first);
	}
	this->item_conf.item_blacklist.clear();
	this->item_conf.only_pick_blacklist = 0;
	this->save();
	f.close();
}

void sdk::game::c_pickup::setup()
{
	this->toggle = sdk::util::c_config::Instance().get_var(XorStr("pickup"), XorStr("toggle"));
	this->distance = sdk::util::c_config::Instance().get_var(XorStr("pickup"), XorStr("range"));
	this->delay = sdk::util::c_config::Instance().get_var(XorStr("pickup"), XorStr("delay"));
}

void sdk::game::c_pickup::save()
{
	auto j = nlohmann::json();
	nlohmann::to_json(j, this->item_conf);
	std::ofstream f(this->item_conf_file);
	f << j << "\n";
	f.close();
}

void sdk::game::c_pickup::clear_whitelist()
{
	this->item_conf.item_whitelist.clear();
	this->save();
}

void sdk::game::c_pickup::clear_blacklist()
{
	this->item_conf.item_blacklist.clear();
	this->save();
}

bool sdk::game::c_pickup::add_blacklist(uint32_t vnum)
{
	for (auto a : this->item_conf.item_blacklist) if (a == vnum) return 0;
	this->item_conf.item_blacklist.push_back(vnum);
	this->save();
	return 1;
}

bool sdk::game::c_pickup::add_whitelist(uint32_t vnum)
{
	for (auto a : this->item_conf.item_whitelist) if (a == vnum) return 0;
	this->item_conf.item_whitelist.push_back(vnum);
	this->save();
	return 1;
}

void sdk::game::c_pickup::unblacklist(uint32_t vnum)
{
	for (auto i = 0; i < (int)this->item_conf.item_blacklist.size(); i++)
	{
		auto b = this->item_conf.item_blacklist[i];
		if (b == vnum)
		{
			this->item_conf.item_blacklist.erase(this->item_conf.item_blacklist.begin() + i);
			break;
		}
	}
}

void sdk::game::c_pickup::unwhitelist(uint32_t vnum)
{
	for (auto i = 0; i < (int)this->item_conf.item_whitelist.size(); i++)
	{
		auto b = this->item_conf.item_whitelist[i];
		if (b == vnum)
		{
			this->item_conf.item_whitelist.erase(this->item_conf.item_whitelist.begin() + i);
			break;
		}
	}
}

void sdk::game::c_pickup::set_blacklist_mode(bool mode)
{
	this->item_conf.only_pick_blacklist = mode;
	this->save();
}

bool sdk::game::c_pickup::is_whitelisted(uint32_t vnum)
{
	for (auto a : this->item_conf.item_whitelist) if (a == vnum) return 1;
	return 0;
}

bool sdk::game::c_pickup::is_blacklisted(uint32_t vnum)
{
	for (auto a : this->item_conf.item_blacklist) if (a == vnum) return 1;
	return 0;
}

bool sdk::game::c_pickup::should_loot(uint32_t vnum)
{
	if (this->item_conf.only_pick_blacklist)
	{
		if (this->is_blacklisted(vnum))
		{
			return 1;
		}
		else return 0;
	}
	else
	{
		if (this->is_whitelisted(vnum))
		{
			return 1;
		}
		else return 0;
	}
	return 0;//what
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
		if (!this->should_loot(a.vnum)) continue;
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
		if (!this->should_loot(a.vnum)) continue; 
		auto dst_to_item = sdk::game::chr::c_char::Instance().get_distance(a.pos, last_pos);
		if (dst_to_item < 300) sdk::game::func::c_funcs::Instance().f_SendItemPickUpPacket(network_manager, a.vid);
	}
	if (last_distance > 300) sdk::game::c_waithack::Instance().interpolate_to_pos(last_pos, main_pos);
}

bool sdk::game::c_pickup::has_config()
{
	std::ifstream f(this->item_conf_file);
	if (f.is_open()) { f.close(); return 1; }
	f.close();
	return 0;
}

void sdk::game::c_pickup::load()
{
	std::ifstream f(this->item_conf_file);
	std::string l = "";
	while (std::getline(f, l))
	{
		if (l.empty()) continue;
		if (l.size() < 14) continue;
		auto jparse = nlohmann::json::parse(l);
		auto asobj = jparse.get<json_pickup::s_item_config>();
		this->item_conf = asobj;
	}
	f.close();
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
