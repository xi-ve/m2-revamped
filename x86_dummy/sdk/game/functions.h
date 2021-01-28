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
			}
			class c_funcs : public s<c_funcs>
			{
			public:
				t::t_ConnectToAccountServer f_ConnectToAccountServer = 0;			//CAccountConnector
				t::t_Connect				f_ConnectToGameServer = 0;				//CPythonNetworkStream
				t::t_SetLoginInfo			f_SetLoginInfo_pynet = 0;				//CPythonNetworkStream
				t::t_SetLoginInfo_passcode	f_SetLoginInfo_passcode_pynet = 0;		//CPythonNetworkStream
				t::t_SetLoginInfo			f_SetLoginInfo_accnet = 0;				//CAccountConnector
				t::t_SetLoginInfo_passcode	f_SetLoginInfo_passcode_accnet = 0;		//CAccountConnector
				t::t_SetMarkServer			f_SetMarkServer = 0;					//CPythonNetworkStream
				t::t_SendSelectCharacter	f_SendSelectCharacter = 0;				//CPythonCharacterManager


				uint32_t					o_SendSelectCharacter = 0;
			};
		}
	}
}