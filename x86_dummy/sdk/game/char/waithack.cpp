#include "waithack.h"

void sdk::game::c_waithack::setup()
{
	this->range = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("range"));
	this->targets = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("targets"));
	this->speed = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("speed"));
	this->toggle = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("toggle"));
	this->metins = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("metins"));
	this->anchor = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("anchor"));
	this->mobs_ = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("mobs"));
	this->on_attack = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("on_attack"));
	this->bp_on_attack = sdk::util::c_config::Instance().get_var(XorStr("waithack"), XorStr("bp_on_attack"));

	MH_CreateHook((void*)sdk::game::func::c_funcs::Instance().o_IsAttacking, (void*)sdk::game::hooks::f_IsPlayerAttacking, (void**)&sdk::game::hooks::o_IsPlayerAttacking);
	MH_EnableHook((void*)sdk::game::func::c_funcs::Instance().o_IsAttacking);
}

int sdk::game::c_waithack::get_range()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->range);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_targets()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->targets);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_speed()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->speed);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_toggle()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->toggle);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_anchor()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->anchor);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_mobs()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->mobs_);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_metins()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->metins);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_on_attack()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->on_attack);
	return std::stoi(r->container.c_str());
}

int sdk::game::c_waithack::get_bp_on_attack()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->bp_on_attack);
	return std::stoi(r->container.c_str());
}

void sdk::game::c_waithack::work()
{
	if (!this->get_toggle()) return;
	auto actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!actor) return;
	if (this->get_on_attack() && !sdk::game::func::c_funcs::Instance().f_IsAttacking(actor)) return;
	if (GetTickCount64() < this->attack_timeout) return;
	this->attack_timeout = GetTickCount64() + (1500 / this->get_speed());
	if (!this->populate()) return;
	this->selective_attack();
}

void sdk::game::c_waithack::force_position(float x, float y)
{
	sdk::util::metin_structs::Point2D p(x,y);
	p.absoluteY();
	auto net_base = sdk::game::c_utils::Instance().baseclass_networking();
	if (!net_base) return;
	sdk::game::func::c_funcs::Instance().f_SendCharacterStatePacket(net_base, p, 0.5, 2, 0);
	sdk::game::func::c_funcs::Instance().f_SendCharacterStatePacket(net_base, p, 0.5, 0, 0);
}

void sdk::game::c_waithack::interpolate_to_pos(vec from, vec to)
{
	auto dst = sdk::game::chr::c_char::Instance().get_distance(from, to);
	auto steps = (float)dst / 1000.f;
	steps = std::roundf(steps);
	if (!steps) steps = 1;
	for (auto i = 0; i <= steps; ++i)
	{
		auto t = (double)(i / steps);
		double x = (1.0 - t) * from.x + t * to.x;
		double y = (1.0 - t) * from.y + t * to.y;
		this->force_position((float)x, (float)y);
	}
}

bool sdk::game::c_waithack::should_attack(uint32_t a)
{
	if (sdk::game::chr::c_char::Instance().is_dead_actor(a)) return 0;
	auto type = sdk::game::chr::c_char::Instance().get_type(a);
	if (type != 0 && type != 2 && type != 15) return false;
	if (this->get_metins() && type == 2) return false;
	if (this->get_mobs() && type == 0) return false;
	return 1;
}

bool sdk::game::c_waithack::populate()
{
	this->mobs.clear();
	this->main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!this->main_actor) return 0;

	auto main_pos = sdk::game::chr::c_char::Instance().get_pos(this->main_actor);
	if (!main_pos.valid()) return 0;

	auto alive_targets = sdk::game::chr::c_char::Instance().get_alive();
	if (!alive_targets.size()) return 0;

	/*											  (f)ptr	 (s)vid		*/
	auto alive_attackables = std::vector<std::pair<uint32_t, uint32_t>>();

	for (auto&& target : alive_targets)
	{
		auto obj = (uint32_t)target.second;
		if (!obj || obj == main_actor || !this->should_attack(obj)) continue;
		auto obj_pos = sdk::game::chr::c_char::Instance().get_pos(obj);
		if (!obj_pos.valid()) continue;
		auto dst_to_us = sdk::game::chr::c_char::Instance().get_distance(obj_pos, main_pos);
		if (dst_to_us >= this->get_range()) continue;
		alive_attackables.push_back({obj, target.first});
	}

	if (alive_attackables.empty()) return 0;

	/*		scan run until all actors are depleted		*/
	while (alive_attackables.size())
	{
		auto obj_dst_to_me_last = 999999.f; auto best_taget = std::pair<uint32_t, uint32_t>(); vec best_target_pos;
		for (auto obj : alive_attackables)
		{
			if (!obj.first) continue;
			auto obj_pos = sdk::game::chr::c_char::Instance().get_pos(obj.first);
			auto dst_to_me = sdk::game::chr::c_char::Instance().get_distance(obj_pos, main_pos);
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
			auto obj_pos = sdk::game::chr::c_char::Instance().get_pos(obj.first);
			auto dst_to_close_target = sdk::game::chr::c_char::Instance().get_distance(obj_pos, best_target_pos);
			if (dst_to_close_target > this->get_anchor()) continue;
			mob_group.mobs.push_back(obj.second);
			if ((int)mob_group.mobs.size() >= this->get_targets()) break;
			/*			mark actor as used up		 */	
			obj.first = 0;
		}

		this->mobs.push_back(mob_group);
		//sdk::util::c_log::Instance().duo("[ found group of mobs (%i) lead by %04x ]\n", mob_group.mobs.size(), mob_group.vid);
		if ((int)this->mobs.size() >= this->get_targets()) break;
	}

	//sdk::util::c_log::Instance().duo("[ total of %i groups ]\n", this->mobs.size());

	return 1;
}

void sdk::game::c_waithack::selective_attack()
{
	if (!this->mobs.size()) return;

	auto main_pos = sdk::game::chr::c_char::Instance().get_pos(this->main_actor);
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();

	if (!network_base) return;

	for (auto&& obj : this->mobs)
	{
		if (!obj.vid || obj.mobs.empty()) continue;
		for (auto&& mob : obj.mobs)
		{
			auto mob_instance = sdk::game::chr::c_char::Instance().get_instance(mob);
			if (!mob_instance || sdk::game::chr::c_char::Instance().is_dead_actor(mob_instance)) continue;
			auto mob_pos = sdk::game::chr::c_char::Instance().get_pos(mob_instance);
			if (!mob_pos.valid()) continue;
			auto mob_dst_to_me = sdk::game::chr::c_char::Instance().get_distance(mob_pos, main_pos);
			if (mob_dst_to_me > 300) this->interpolate_to_pos(main_pos, mob_pos);
			sdk::game::func::c_funcs::Instance().f_SendAttackPacket(network_base, 0, mob);
			if (mob_dst_to_me > 300) this->interpolate_to_pos(mob_pos, main_pos);
		}
	}
}

decltype(sdk::game::hooks::o_IsPlayerAttacking) sdk::game::hooks::o_IsPlayerAttacking = 0;
bool __fastcall sdk::game::hooks::f_IsPlayerAttacking(uint32_t base)
{
	if (sdk::game::c_waithack::Instance().get_bp_on_attack()) return 1;
	else return sdk::game::hooks::o_IsPlayerAttacking(base);
}
