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
