#include "item_gather.h"

void sdk::game::item::c_item_gather::setup()
{
}

void sdk::game::item::c_item_gather::work()
{
	auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!main_actor) return;

	auto item_instance = sdk::game::c_utils::Instance().baseclass_python_item();
	if (!item_instance) return;

	auto ground_items = *(std::map<uint32_t, uint32_t*>*)(item_instance + 0x4);
	if (ground_items.empty())
	{
		this->items.clear();
		return;
	}

	auto new_entries = std::vector<s_item>();

	for (auto a : ground_items)
	{
		auto p = (uint32_t)a.second;
		auto vid = a.first;
		if (!vid || !p) continue;
		auto owner = *(std::string*)(p + sdk::game::item_offsets::off_OWNER_NAME);
		if (owner.size()) if (owner.compare(sdk::game::chr::c_char::Instance().get_name(main_actor))) continue;
		auto pos = vec();
		pos.x = *(float*)((uint32_t)p + 0x8);
		pos.y = *(float*)((uint32_t)p + 0xC);
		pos.z = *(float*)((uint32_t)p + 0x10);
		if (!pos.valid()) continue;
		auto s = s_item();
		s.vid = vid;
		s.address = p;
		s.owner = owner;
		s.pos = pos;
		s.vnum = *(uint32_t*)(p + 0x4);
		s.drop_time = GetTickCount64();
		new_entries.push_back(s);
	}

	if (this->items.empty()) this->items = new_entries;
	else
	{
		for (auto&& a : new_entries)
		{
			auto contains = false;
			for (auto&& b : this->items) if (b.vid == a.vid) { contains = true; break; }
			if (contains) continue;
			this->items.push_back(a);
		}
		for (auto i = 0; i < (int)this->items.size(); i++)
		{
			//delete old instances
			auto a = this->items[i];
			if (ground_items.contains(a.vid) != 0) continue;
			this->items.erase(this->items.begin() + i);
		}
	}
}
