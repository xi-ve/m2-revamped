#include "utils.h"

uint32_t sdk::game::c_utils::baseclass_networking()
{
    return *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonNetworkStream);
}

uint32_t sdk::game::c_utils::baseclass_account_connector()
{
    return *(uint32_t*)(sdk::game::pointer_offsets::off_CAccountConnector);
}

uint32_t sdk::game::c_utils::baseclass_character_manager()
{
    return *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonCharacterManager);
}

uint32_t sdk::game::c_utils::baseclass_python_player()
{
    return *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonPlayer);
}

uint32_t sdk::game::c_utils::baseclass_event_handler()
{
    auto bp = sdk::game::chr::c_char::Instance().get_main_actor();
    if (!bp) return 0;
    auto gi = sdk::game::chr::c_char::Instance().get_graphic_thing(bp);
    if (!gi) return 0;
    return *(uint32_t*)(gi + sdk::game::actor_offsets::off_SHOULD_SKIP_COLLISION + 0x1c);
}

uint32_t sdk::game::c_utils::baseclass_python_item()
{
	return *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonItem);
}
