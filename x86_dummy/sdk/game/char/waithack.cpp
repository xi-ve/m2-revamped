#include "waithack.h"

void sdk::game::c_waithack::setup()
{
	this->range = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("range"));
	this->targets = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("targets"));
	this->speed = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("speed"));
	this->toggle = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("toggle"));
	this->metins = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("metins"));
	this->anchor = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("anchor"));
	this->mobs_ = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("mobs"));
	this->player = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("player"));
	this->on_attack = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("on_attack"));
	this->boost = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("boost"));
	this->bp_on_attack = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("bp_on_attack"));
	this->bow_mode = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("bow_mode"));
	this->skill = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("skill"));
	this->minatk = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("minatk"));
	this->magnet = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("magnet"));
	this->force = util::c_config::Instance().get_var(XorStr("waithack"), XorStr("force"));


	MH_CreateHook((void*)func::c_funcs::Instance().o_IsAttacking, static_cast<void*>(hooks::f_IsPlayerAttacking),
	              (void**)&
	              hooks::o_IsPlayerAttacking);
	MH_EnableHook((void*)func::c_funcs::Instance().o_IsAttacking);
}

int sdk::game::c_waithack::get_range()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->range);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_magnet()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->magnet);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_force()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->force);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_targets()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->targets);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_speed()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->speed);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_toggle()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->toggle);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_anchor()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->anchor);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_mobs()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->mobs_);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_metins()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->metins);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_player()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->player);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_on_attack()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->on_attack);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_bp_on_attack()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->bp_on_attack);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_boost()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->boost);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_bow_mode()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->bow_mode);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_skill()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->skill);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_minatk()
{
	auto r = static_cast<util::json_cfg::s_config_value*>(this->minatk);
	return std::stoi(r->container.c_str());
}

void sdk::game::c_waithack::work()
{
	if (!this->get_toggle()) return;
	auto actor = chr::c_char::Instance().get_main_actor();
	if (!actor) return;
	if (this->get_on_attack() && !func::c_funcs::Instance().f_IsAttacking(actor)) return;
	if (GetTickCount64() < this->attack_timeout) return;
	this->attack_timeout = GetTickCount64() + (1500 / this->get_speed());
	if (!this->populate()) return;
	this->selective_attack();
}

void sdk::game::c_waithack::force_position(float x, float y)
{
	util::metin_structs::Point2D p(x, y);
	p.absoluteY();
	auto net_base = c_utils::Instance().baseclass_networking();
	if (!net_base) return;
	func::c_funcs::Instance().f_SendCharacterStatePacket(net_base, p, 0.5, 2, 0); //0 ok too
	func::c_funcs::Instance().f_SendCharacterStatePacket(net_base, p, 0.5, 0, 0);
}

void sdk::game::c_waithack::interpolate_to_pos(vec from, vec to)
{
	auto dst = chr::c_char::Instance().get_distance(from, to);
	auto steps = static_cast<float>(dst) / static_cast<float>(this->get_anchor());
	steps = std::roundf(steps);
	if (!steps) steps = 1;
	for (auto i = 0; i <= steps; ++i)
	{
		auto t = static_cast<double>(i / steps);
		double x = (1.0 - t) * from.x + t * to.x;
		double y = (1.0 - t) * from.y + t * to.y;
		this->force_position(static_cast<float>(x), static_cast<float>(y));
	}
}

void sdk::game::c_waithack::interpolate_to_pos_with_mob(uint32_t netbase, DWORD mob, vec from, vec to)
{
	auto dst = chr::c_char::Instance().get_distance(from, to);
	auto steps = static_cast<float>(dst) / static_cast<float>(this->get_anchor());
	steps = std::roundf(steps);
	if (!steps) steps = 1;
	for (auto i = 0; i <= steps; ++i)
	{
		auto t = static_cast<double>(i / steps);
		double x = (1.0 - t) * from.x + t * to.x;
		double y = (1.0 - t) * from.y + t * to.y;
		ByteBuffer kPacketSync;
		kPacketSync.putT<BYTE>(0x8);
		kPacketSync.putT<WORD>(15);
		unsigned int sync_pos_count_limit = 1;

		ByteBuffer kSyncPos;
		long mob_x = x;
		long mob_y = abs(y);
		func::c_funcs::Instance().f___LocalPositionToGlobalPosition(netbase, mob_x, mob_y);

		kSyncPos.putT<DWORD>(mob);
		kSyncPos.putT<long>(mob_x);
		kSyncPos.putT<long>(mob_y);
		sdk::game::c_exploit::Instance().Send(netbase, 3, &kPacketSync.buf[0]);
		sdk::game::c_exploit::Instance().Send(netbase, 12, &kSyncPos.buf[0]);
		func::c_funcs::Instance().f_SendSequence(netbase);
		Sleep(100);
		this->force_position(static_cast<float>(x), static_cast<float>(y));
	}
}




bool sdk::game::c_waithack::should_attack(uint32_t a)
{
	if (chr::c_char::Instance().is_dead_actor(a)) return false;
	auto type = chr::c_char::Instance().get_type(a);
	if (type != 0 && type != 2 && type != 15 && type != 6) return false;
	if (this->get_metins() && type == 2) return false;
	if (this->get_mobs() && type == 0) return false;
	if (this->get_player() && type == 6) return false;
	return true;
}

bool sdk::game::c_waithack::populate()
{
	this->mobs.clear();
	this->main_actor = chr::c_char::Instance().get_main_actor();
	if (!this->main_actor) return false;

	auto main_pos = chr::c_char::Instance().get_pos(this->main_actor);
	if (!main_pos.valid()) return false;

	auto alive_targets = chr::c_char::Instance().get_alive();
	if (!alive_targets.size()) return false;

	/*											  (f)ptr	 (s)vid		*/
	auto alive_attackables = std::vector<std::pair<uint32_t, uint32_t>>();

	for (auto&& target : alive_targets)
	{
		auto obj = (uint32_t)target.second;
		if (!obj || obj == main_actor || !this->should_attack(obj)) continue;
		auto obj_pos = chr::c_char::Instance().get_pos(obj);
		if (!obj_pos.valid()) continue;
		auto dst_to_us = chr::c_char::Instance().get_distance(obj_pos, main_pos);
		if (dst_to_us >= this->get_range()) continue;
		alive_attackables.push_back({obj, target.first});
	}

	if (alive_attackables.empty()) return false;

	/*		scan run until all actors are depleted		*/
	while (alive_attackables.size())
	{
		auto obj_dst_to_me_last = 999999.f;
		auto best_taget = std::pair<uint32_t, uint32_t>();
		vec best_target_pos;
		for (auto obj : alive_attackables)
		{
			if (!obj.first) continue;
			auto obj_pos = chr::c_char::Instance().get_pos(obj.first);
			auto dst_to_me = chr::c_char::Instance().get_distance(obj_pos, main_pos);
			if (dst_to_me < obj_dst_to_me_last)
			{
				best_taget = obj;
				obj_dst_to_me_last = dst_to_me;
				best_target_pos = obj_pos;
			}
		}

		/*		 seems like we are out of actors near us?		 */
		if (!best_taget.first) break;

		auto mob_group = s_mobs_area_point();
		mob_group.vid = best_taget.second;

		for (auto&& obj : alive_attackables)
		{
			if (!obj.first) continue;
			auto obj_pos = chr::c_char::Instance().get_pos(obj.first);
			auto dst_to_close_target = chr::c_char::Instance().get_distance(obj_pos, best_target_pos);
			if (dst_to_close_target > this->get_anchor()) continue;
			mob_group.mobs.push_back(obj.second);
			if (static_cast<int>(mob_group.mobs.size()) >= this->get_targets()) break;
			if (!obj.second) continue;


			/*			mark actor as used up		 */
			obj.first = 0;
		}

		this->mobs.push_back(mob_group);
		if (static_cast<int>(this->mobs.size()) >= this->get_targets()) break;
	}
	return true;
}

void sdk::game::c_waithack::selective_attack()
{
	if (!this->mobs.size()) return;

	auto main_pos = chr::c_char::Instance().get_pos(this->main_actor);
	auto network_base = c_utils::Instance().baseclass_networking();

	if (!network_base) return;

	for (auto&& obj : this->mobs)
	{
		if (!obj.vid || obj.mobs.empty()) continue;
		for (auto&& mob : obj.mobs)
		{
			auto mob_instance = chr::c_char::Instance().get_instance(mob);

			if (!mob_instance || chr::c_char::Instance().is_dead_actor(mob_instance)) continue;
			auto mob_pos = chr::c_char::Instance().get_pos(mob_instance);
			if (!mob_pos.valid()) continue;


			auto mob_dst_to_me = chr::c_char::Instance().get_distance(mob_pos, main_pos);

			if (mob_dst_to_me > this->get_minatk()) this->interpolate_to_pos(main_pos, mob_pos);
			if (this->get_magnet())
			{
				auto net_base = c_utils::Instance().baseclass_networking();
				if (!net_base) continue;
				if (this->get_force())
				{
					auto new_pos = mob_pos;
					new_pos.x += 2000;
					new_pos.y -= 2000;
					this->interpolate_to_pos_with_mob(net_base, mob, mob_pos, new_pos);
					chr::c_char::Instance().set_pos(mob_instance, new_pos.x, new_pos.y, new_pos.z);
					this->interpolate_to_pos(new_pos, main_pos);
				}
				else
				{
					this->interpolate_to_pos_with_mob(net_base, mob, mob_pos, main_pos);
					chr::c_char::Instance().set_pos(mob_instance, main_pos.x, main_pos.y, main_pos.z);
				}
				continue;
			}
			if (this->get_bow_mode() == 0)
			{
				if (func::c_funcs::Instance().f_SendAttackPacket)
				{
					func::c_funcs::Instance().f_SendAttackPacket(network_base, 0, mob);
					if (this->get_boost()) func::c_funcs::Instance().f_SendAttackPacket(network_base, 0, mob);
				}
				else
				{
					auto event_base = c_utils::Instance().baseclass_event_handler();
					auto mob_graph = chr::c_char::Instance().get_graphic_thing(mob_instance);
					func::c_funcs::Instance().f_OnHit(event_base, 0, mob_graph, TRUE);
					if (this->get_boost()) func::c_funcs::Instance().f_OnHit(event_base, 0, mob_graph, TRUE);
				}
			}
			else
			{
				util::metin_structs::Point2D p(mob_pos.x, mob_pos.y);
				p.absoluteY();

				func::c_funcs::Instance().f_SendFlyTargetingPacket(network_base, mob, p);

				func::c_funcs::Instance().f_SendShootPacket(network_base, this->get_skill());
				if (this->get_boost()) func::c_funcs::Instance().f_SendShootPacket(network_base, this->get_skill());
			}
			if (mob_dst_to_me > this->get_minatk()) this->interpolate_to_pos(mob_pos, main_pos);
		}
	}
}

decltype(sdk::game::hooks::o_IsPlayerAttacking) sdk::game::hooks::o_IsPlayerAttacking = nullptr;

bool __fastcall sdk::game::hooks::f_IsPlayerAttacking(uint32_t base)
{
	if (c_waithack::Instance().get_bp_on_attack() && chr::c_char::Instance().get_main_actor() == base) return true;
	return o_IsPlayerAttacking(base);
}
