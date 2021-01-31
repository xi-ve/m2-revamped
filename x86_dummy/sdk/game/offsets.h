#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace function_offsets
		{

		}
		namespace pointer_offsets
		{
			extern uint32_t		off_CAccountConnector;
			extern uint32_t		off_CPythonNetworkStream;
			extern uint32_t		off_CPythonCharacterManager;
			extern uint32_t		off_CPythonPlayer;
		}
		namespace actor_offsets
		{
			extern uint32_t		off_FAINT;
			extern uint32_t		off_SLEEP;
			extern uint32_t		off_RESIST;
			extern uint32_t		off_STUN;
			extern uint32_t		off_DEAD;
			extern uint32_t		off_WALKING;
			extern uint32_t		off_GRAPHIC_THING;
			extern uint32_t		off_ACTOR_TYPE;
			extern uint32_t		off_COMBO_INDEX;
			extern uint32_t		off_VTABLE_GET_MAIN;
			extern uint32_t		off_VIRTUAL_ID;
			extern uint32_t		off_SHOULD_SKIP_COLLISION;
			extern uint32_t		off_NAME;
			extern uint32_t		off_POSITION;
		}
		namespace connection_offsets
		{
			extern uint32_t		off_IP;
			extern uint32_t		off_PORT;
			extern uint32_t		off_SOCKET;
			extern uint32_t		off_USERNAME;
			extern uint32_t		off_PASSWORD;
			extern uint32_t		off_PASSCODE;
			extern uint32_t		off_NETWORKING_PHASE;
			extern uint32_t		off_IS_CONNECTED;
			extern uint32_t		off_PACKET_SEQUENCE_MODE;
			extern uint32_t		off_SELECT_CHAR_IDX;
		}
	}
}