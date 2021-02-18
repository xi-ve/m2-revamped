#include "item_manager.h"
using namespace std::chrono;
void sdk::game::item::c_item_manager::setup()
{
}

bool sdk::game::item::c_item_manager::grab()
{
	auto manager = sdk::game::c_utils::Instance().baseclass_item_manager();
	if (!manager) return 0;

	auto map = std::map<uint32_t, uint32_t*>();
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Anoria2") ||
		strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Hybrid2"))
	{
		map = *(std::map<uint32_t, uint32_t*>*)(manager + 0x8);
		if (map.empty()) return 0;
	}
	else
	{
		map = *(std::map<uint32_t, uint32_t*>*)(manager + 0x4);
		if (map.empty()) return 0;
	}

	std::this_thread::sleep_for(5s);

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Anoria2") ||
		strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Hybrid2"))
	{
		map = *(std::map<uint32_t, uint32_t*>*)(manager + 0x8);
		if (map.empty()) return 0;
	}
	else
	{
		map = *(std::map<uint32_t, uint32_t*>*)(manager + 0x4);
		if (map.empty()) return 0;
	}

	sdk::util::c_log::Instance().duo("[ found %i items ]\n", map.size());

	std::ofstream items("M2++_ITEMS.TXT");
	for (auto a : map)
	{
		auto p = (uint32_t)a.second;
		if (!p) continue;

		char name[26] = "\0";
		memcpy(name, (void*)(p + sdk::game::item_offsets::off_ITEM_NAME), 24);
		if (!name || strlen(name) <= 2) continue;

		this->item_names[a.first] = name;

		items << sdk::util::c_log::Instance().string("[ item ptr: %04x, item name: %s, vnum: %i ]\n", p, name, a.first);
	}
	items.close();

	this->item_map = map;
	this->did_grab = 1;

	if (sdk::game::c_pickup::Instance().has_config()) sdk::game::c_pickup::Instance().load();
	else sdk::game::c_pickup::Instance().make_clean_conf();

	sdk::game::c_pickup::Instance().save();

	return 1;
}
