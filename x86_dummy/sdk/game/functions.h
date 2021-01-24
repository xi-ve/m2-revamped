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
				typedef void(__thiscall* t_SetMarkServer)(uint32_t, const char*, UINT);
				typedef bool(__thiscall* t_SendSelectCharacter)(uint32_t, uint32_t);
			}
			class c_funcs : public s<c_funcs>
			{
			public:
				t::t_ConnectToAccountServer f_ConnectToAccountServer = 0;	//CAccountConnector
				t::t_Connect				f_Connect = 0;					//CPythonNetworkStream
				t::t_SetLoginInfo			f_SetLoginInfo_pynet = 0;		//CPythonNetworkStream
				t::t_SetLoginInfo			f_SetLoginInfo_accnet = 0;		//CAccountConnector
				t::t_SetMarkServer			f_SetMarkServer = 0;			//CPythonNetworkStream
				t::t_SendSelectCharacter	f_SendSelectCharacter = 0;		//CPythonCharacterManager
			};
		}
	}
}