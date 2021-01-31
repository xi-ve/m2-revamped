#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace func
		{
			namespace t
			{
				typedef bool(__thiscall* t_ConnectToAccountServer)(uint32_t, const char*, int, const char*, int);
				typedef void(__thiscall* t_Connect)(uint32_t, uint32_t);
				typedef void(__thiscall* t_SetLoginInfo)(uint32_t, const char*, const char*);
				typedef void(__thiscall* t_SetLoginInfo_passcode)(uint32_t, const char*, const char*, const char*);
				typedef void(__thiscall* t_SetMarkServer)(uint32_t, const char*, UINT);
				typedef bool(__thiscall* t_SendSelectCharacter)(uint32_t, BYTE);
				typedef void(__thiscall* t_SendCharacterStatePacket)(uint32_t, const sdk::util::metin_structs::Point2D&, float, UINT, UINT);
				typedef bool(__thiscall* t_SendAttackPacket)(uint32_t, UINT, uint32_t);
				typedef bool(__thiscall* t_IsAttacking)(uint32_t);
			}
			class c_funcs : public s<c_funcs>
			{
			public:
				t::t_ConnectToAccountServer		f_ConnectToAccountServer = 0;			//CAccountConnector
				t::t_Connect					f_ConnectToGameServer = 0;				//CPythonNetworkStream
				t::t_SetLoginInfo				f_SetLoginInfo_pynet = 0;				//CPythonNetworkStream
				t::t_SetLoginInfo_passcode		f_SetLoginInfo_passcode_pynet = 0;		//CPythonNetworkStream
				t::t_SetLoginInfo				f_SetLoginInfo_accnet = 0;				//CAccountConnector
				t::t_SetLoginInfo_passcode		f_SetLoginInfo_passcode_accnet = 0;		//CAccountConnector
				t::t_SetMarkServer				f_SetMarkServer = 0;					//CPythonNetworkStream
				t::t_SendSelectCharacter		f_SendSelectCharacter = 0;				//CPythonCharacterManager
				t::t_SendCharacterStatePacket	f_SendCharacterStatePacket = 0;			//CPythonNetworkStream
				t::t_SendAttackPacket			f_SendAttackPacket = 0;					//CPythonNetworkStream
				t::t_IsAttacking				f_IsAttacking = 0;						//CPythonPlayer

				uint32_t						o_SendSelectCharacter = 0;
				uint32_t						o_SendCharacterStatePacket = 0;
				uint32_t						o_IsAttacking = 0;
			};
		}
	}
}