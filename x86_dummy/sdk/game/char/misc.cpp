#include "misc.h"

void sdk::game::chr::c_misc::setup()
{
	this->wallhack = sdk::util::c_config::Instance().get_var(XorStr("misc"), XorStr("wallhack"));
}

int sdk::game::chr::c_misc::get_wallhack()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->wallhack);
	return std::stoi(r->container.c_str());
}

void sdk::game::chr::c_misc::no_coll()
{
	if (!this->get_wallhack())
	{
		if (this->last_wallhack)
		{
			auto graph = sdk::game::chr::c_char::Instance().get_graphic_thing(this->main_actor);
			if (!graph) return;
			*(BYTE*)(graph + sdk::game::actor_offsets::off_SHOULD_SKIP_COLLISION) = 0;
			this->last_wallhack = 0;
		}
		return;
	}
	auto graph = sdk::game::chr::c_char::Instance().get_graphic_thing(this->main_actor);
	if (!graph) return;
	*(BYTE*)(graph + sdk::game::actor_offsets::off_SHOULD_SKIP_COLLISION) = 1;
	this->last_wallhack = 1;
}

void sdk::game::chr::c_misc::work()
{
	this->main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!this->main_actor) return;
	this->no_coll();
}