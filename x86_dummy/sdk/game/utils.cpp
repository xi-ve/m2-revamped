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
