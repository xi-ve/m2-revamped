#include "utils.h"

uint32_t sdk::game::c_utils::baseclass_networking()
{
    return *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonNetworkStream);
}

uint32_t sdk::game::c_utils::baseclass_account_connector()
{
    return *(uint32_t*)(sdk::game::pointer_offsets::off_CAccountConnector);
}
