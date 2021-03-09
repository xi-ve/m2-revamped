#pragma once
namespace sdk
{
	namespace game
	{
		namespace item_offsets
		{
			extern uint32_t		off_OWNER_NAME;
			extern uint32_t		off_ITEM_NAME;
		}
		namespace background
		{
			extern uint32_t		off_CUR_MAP_NAME;
		}
		namespace pack_offsets
		{
			extern uint32_t		off_CMAPPED_FILE_SIZE;
			extern uint32_t		off_CEterPackManager_GET;
		}
		namespace minimap_offsets
		{
			extern uint32_t		off_TEXTURE;
			extern uint32_t		off_SHOW_ATLAS;
			extern uint32_t		off_ATLAS_INSTANCE;
		}
		namespace pointer_offsets
		{
			extern uint32_t		off_CAccountConnector;
			extern uint32_t		off_CPythonNetworkStream;
			extern uint32_t		off_CPythonCharacterManager;
			extern uint32_t		off_CPythonPlayer;
			extern uint32_t		off_CPythonPlayerEventHandler;
			extern uint32_t		off_CPythonItem;
			extern uint32_t		off_CItemManager;
			extern uint32_t		off_CPythonBackgroundManager;
			extern uint32_t		off_CPythonMiniMap;
			extern uint32_t		off_CEterPackManager;
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
		}
	}
}