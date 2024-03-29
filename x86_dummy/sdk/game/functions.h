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
				typedef bool(__thiscall* t_SendShootPacket)(uint32_t, UINT);
				typedef bool(__thiscall* t_SendFlyTargetingPacket)(uint32_t, DWORD, const sdk::util::metin_structs::Point2D&);
				typedef bool(__thiscall* t_IsAttacking)(uint32_t);
				typedef void(__thiscall* t_OnHit)(uint32_t, UINT, uint32_t, BOOL);
				typedef bool(__thiscall* t_SendItemPickUpPacket)(uint32_t, uint32_t);
				typedef bool(__thiscall* t_EterPackGet)(uint32_t, uint32_t, const char*, const void**);
				typedef bool(__thiscall* t_CPythonApplicationProcess)(uint32_t);
				typedef bool(__thiscall* t_SendSequence)(uint32_t);
				typedef bool(__thiscall* t_Send)(uint32_t, int, const char *);
				typedef bool(__thiscall* t___SendInternalBuffer)(uint32_t);
				typedef void(__thiscall* t___LocalPositionToGlobalPosition)(uint32_t, LONG&, LONG&);
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
				t::t_SendShootPacket			f_SendShootPacket = 0;					//CPythonNetworkStream
				t::t_SendSequence				f_SendSequence = 0;					//CPythonNetworkStream
				t::t_Send						f_Send = 0;					//CPythonNetworkStream
				t::t___LocalPositionToGlobalPosition f___LocalPositionToGlobalPosition = 0;
				t::t___SendInternalBuffer			f___SendInternalBuffer = 0;
				t::t_SendFlyTargetingPacket		f_SendFlyTargetingPacket = 0;			//CPythonNetworkStream
				t::t_IsAttacking				f_IsAttacking = 0;						//CPythonPlayer
				t::t_OnHit						f_OnHit = 0;							//CPythonPlayerEventHandler
				t::t_SendItemPickUpPacket		f_SendItemPickUpPacket = 0;				//CPythonNetworkStream
				t::t_EterPackGet				f_EterPackGet = 0;						//CEterPackManager
				t::t_CPythonApplicationProcess	f_CPythonApplicationProcess = 0;
				uint32_t						o_SendSelectCharacter = 0;
				uint32_t						o_SendCharacterStatePacket = 0;
				uint32_t						o_IsAttacking = 0;
				uint32_t						o_SendHitEvent = 0;						//for servers that use lalaker patch, inline dmg
				uint32_t						o_SendShootPacket = 0;
			

				uint32_t						o_CPythonApplicationProcess = 0;
			};
		}
	}
}