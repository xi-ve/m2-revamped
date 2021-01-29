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
	return this->dead_actors;
}

std::map<uint32_t, uint32_t*> sdk::game::chr::c_char::get_alive()
{
	return this->alive_actors;
}

uint32_t sdk::game::chr::c_char::get_main_actor()
{
	auto player_base = sdk::game::c_utils::Instance().baseclass_python_player() + 4;
	if (!player_base || player_base == 4) return 0;
	return (uint32_t)(*(int(__thiscall**)(int))(*(DWORD*)player_base + sdk::game::actor_offsets::off_VTABLE_GET_MAIN))(player_base);
}
