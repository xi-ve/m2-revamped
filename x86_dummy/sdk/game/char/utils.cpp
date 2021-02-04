#include <inc.h>

void sdk::game::chr::c_char::setup()
{
}

void sdk::game::chr::c_char::update()
{
	auto char_base = *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonCharacterManager);
	if (!char_base) return;

	auto map_alive = *(std::map<uint32_t, uint32_t*>*)(char_base + 0x20);
	auto list_dead = *(std::list<uint32_t*>*)(char_base + 0x28);

	this->alive_actors = map_alive;
	this->dead_actors = list_dead;
}

std::list<uint32_t*> sdk::game::chr::c_char::get_dead()
{
	auto char_base = *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonCharacterManager);
	if (!char_base) return {};

	if (use_alt_mode_list) return *(std::list<uint32_t*>*)(char_base + 0x2C);
	else return *(std::list<uint32_t*>*)(char_base + 0x28);
}

std::map<uint32_t, uint32_t*> sdk::game::chr::c_char::get_alive()
{
	auto char_base = *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonCharacterManager);
	if (!char_base) return {};

	if (use_alt_mode_list) return *(std::map<uint32_t, uint32_t*>*)(char_base + 0x24);
	else return *(std::map<uint32_t, uint32_t*>*)(char_base + 0x20);
}

uint32_t sdk::game::chr::c_char::get_main_actor()
{
	auto player_base = sdk::game::c_utils::Instance().baseclass_python_player() + 4;
	if (!player_base || player_base == 4) return 0;
	return (uint32_t)(*(int(__thiscall**)(int))(*(DWORD*)player_base + sdk::game::actor_offsets::off_VTABLE_GET_MAIN))(player_base);
}

uint32_t sdk::game::chr::c_char::get_graphic_thing(uint32_t instance_base)
{
	return instance_base + sdk::game::actor_offsets::off_GRAPHIC_THING;
}

bool sdk::game::chr::c_char::is_dead_actor(uint32_t instance_base)
{
	auto graph = this->get_graphic_thing(instance_base);
	if (!graph) return 0;

	return *(uint8_t*)(graph + sdk::game::actor_offsets::off_DEAD);
}

uint32_t sdk::game::chr::c_char::get_type(uint32_t instance_base)
{
	auto graph = this->get_graphic_thing(instance_base);
	if (!graph) return 0;

	return *(uint32_t*)(graph + sdk::game::actor_offsets::off_ACTOR_TYPE);
}

sdk::util::math::c_vector3 sdk::game::chr::c_char::get_pos(uint32_t instance_base)
{
	auto graph = this->get_graphic_thing(instance_base);
	if (!graph) return {};

	vec r;
	r.x = *(float*)(graph + sdk::game::actor_offsets::off_POSITION);
	r.y = *(float*)(graph + sdk::game::actor_offsets::off_POSITION + 4);
	r.z = *(float*)(graph + sdk::game::actor_offsets::off_POSITION + 8);

	return r;
}

float sdk::game::chr::c_char::get_distance(vec f, vec t)
{
	auto a = f.x - t.x; auto b = f.y - t.y;
	return sqrtf(a * a + b * b);
}

std::string sdk::game::chr::c_char::get_name(uint32_t instance_base)
{
	return *(std::string*)(instance_base + sdk::game::actor_offsets::off_NAME);
}

bool sdk::game::chr::c_char::is_attackable(uint32_t instance_base)
{
	auto type = this->get_type(instance_base);
	if (type == 0 || type == 2) return 1;
	return 0;
}

uint32_t sdk::game::chr::c_char::get_vid(uint32_t instance_base)
{
	auto graph = this->get_graphic_thing(instance_base);
	if (!graph) return {};

	return *(uint32_t*)(graph + sdk::game::actor_offsets::off_VIRTUAL_ID);
}

uint32_t sdk::game::chr::c_char::get_instance(uint32_t vid)
{
	for (auto a : this->get_alive()) if (a.first == vid) return (uint32_t)a.second;
	return 0;
}
