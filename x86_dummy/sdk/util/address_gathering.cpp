#include "address_gathering.h"

void sdk::util::c_address_gathering::setup()
{
	auto r = this->gather_connection_related();
	if (!r)
	{
		sdk::util::c_log::Instance().duo(XorStr("[ failed gather_connection_related ]\n"));
		return;
	}
	r = this->gather_actor_related();
	if (!r)
	{
		sdk::util::c_log::Instance().duo(XorStr("[ failed gather_actor_related ]\n"));
		return;
	}
	r = this->gather_item_related();
	if (!r)
	{
		sdk::util::c_log::Instance().duo(XorStr("[ failed gather_item_related ]\n"));
		return;
	}
	r = this->gather_background_related();
	if (!r)
	{
		sdk::util::c_log::Instance().duo(XorStr("[ failed gather_background_related ]\n"));
		return;
	}
	r = this->gather_pack_related();
	if (!r)
	{
		sdk::util::c_log::Instance().duo(XorStr("[ failed gather_pack_related ]\n"));
		return;
	}
	sdk::util::c_log::Instance().duo(XorStr("[ c_address_gathering::setup completed ]\n"));
	done = true;
}

bool sdk::util::c_address_gathering::error_out(int line)
{
	sdk::util::c_log::Instance().duo(XorStr("[ failed on: %i ]\n"), line);
	return 0;
}

sdk::util::t_asm_res sdk::util::c_address_gathering::find_custom_range(uint32_t adr, uint32_t min, uint32_t max,
                                                                       uint32_t steps,
                                                                       std::vector<std::string> instructions,
                                                                       bool from_max)
{
	if (!from_max)
	{
		for (auto a = min; a < max; a += steps)
		{
			auto f = sdk::util::c_disassembler::Instance().get_custom(adr, 0, a, a + steps, instructions);
			if (f.empty()) continue;
			return f;
		}
	}
	else
	{
		for (auto a = max; a > min; a -= steps)
		{
			auto f = sdk::util::c_disassembler::Instance().get_custom(adr, 0, a, a + steps, instructions);
			if (f.empty()) continue;
			return f;
		}
	}
	return {};
}

uint32_t sdk::util::c_address_gathering::find_singleton_or_instance(uint32_t f)
{
	auto off = (uint32_t)0x0;

	auto singleton_fn = sdk::util::c_fn_discover::Instance().discover_fn(f, 0x8, 0x9, 0, 1, 1, 0, 1);
	if (singleton_fn)
	{
		auto off_inside = sdk::util::c_disassembler::Instance().get_custom(singleton_fn, 0, 0, 0, {XorStr("mov")});
		if (off_inside.empty()) return 0;
		off = off_inside.front();
	}
	else
	{
		auto off_inside = sdk::util::c_disassembler::Instance().get_custom(f, 0, 0, 0, {XorStr("mov")});
		if (off_inside.empty()) return 0;
		off = off_inside.front();
	}

	return off;
}

bool sdk::util::c_address_gathering::gather_connection_related()
{
	auto connectaccountserver_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("ConnectToAccountServer"));
	if (!connectaccountserver_fn) return this->error_out(__LINE__);
	auto CAccountConnector_connect = sdk::util::c_fn_discover::Instance().discover_fn(
		connectaccountserver_fn, 0x50, 0x70, 3);
	if (!CAccountConnector_connect)
	{
		auto CAccountConnector_connect_calls = sdk::util::c_disassembler::Instance().get_calls(
			connectaccountserver_fn, 0, 0, 0);
		CAccountConnector_connect = CAccountConnector_connect_calls[CAccountConnector_connect_calls.size() - 2];
		if (!CAccountConnector_connect) return this->error_out(__LINE__);
	}

	auto CAccountConnector_instance = this->find_singleton_or_instance(connectaccountserver_fn);
	if (!CAccountConnector_instance) return this->error_out(__LINE__);

	auto offsets = sdk::util::c_disassembler::Instance().get_custom(CAccountConnector_connect, 0, 0xA0, 0xFF,
	                                                                {"mov", "add", "push", "lea"});
	if (!((int)offsets.size() >= 2)) return this->error_out(__LINE__);

	auto CNetworkStream__Connect = sdk::util::c_fn_discover::Instance().get_fn(XorStr("error != WSAEWOULDBLOCK"));
	if (!CNetworkStream__Connect) return this->error_out(__LINE__);
	auto CNetworkStream__Connect_off = sdk::util::c_disassembler::Instance().get_custom(
		CNetworkStream__Connect, 0, 0x40, 0xA0, {"mov", "add", "push", "lea"});
	if (!CNetworkStream__Connect_off.size()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_IP = offsets[0];
	sdk::game::connection_offsets::off_PORT = offsets[1];
	sdk::game::connection_offsets::off_SOCKET = CNetworkStream__Connect_off.front();
	sdk::game::pointer_offsets::off_CAccountConnector = CAccountConnector_instance;
	sdk::game::func::c_funcs::Instance().f_ConnectToAccountServer = decltype(sdk::game::func::c_funcs::Instance().
		f_ConnectToAccountServer)(CAccountConnector_connect);

	sdk::util::c_log::Instance().duo(XorStr("[ off_IP        : %04x ]\n"), sdk::game::connection_offsets::off_IP);
	sdk::util::c_log::Instance().duo(XorStr("[ off_PORT      : %04x ]\n"), sdk::game::connection_offsets::off_PORT);
	sdk::util::c_log::Instance().duo(XorStr("[ off_SOCKET    : %04x ]\n"), sdk::game::connection_offsets::off_SOCKET);
	sdk::util::c_log::Instance().duo(XorStr("[ connector fn  : %04x ]\n"), CAccountConnector_connect);
	sdk::util::c_log::Instance().duo(XorStr("[ connector inst: %04x ]\n"), CAccountConnector_instance);

	//

	auto directenter_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("DirectEnter"));
	if (!directenter_fn) return this->error_out(__LINE__);
	auto dodirectenter_fn = sdk::util::c_fn_discover::Instance().discover_fn(directenter_fn, 0x30, 0x65, 2, 0);
	if (!dodirectenter_fn)
	{
		dodirectenter_fn = sdk::util::c_fn_discover::Instance().discover_fn(directenter_fn, 0x30, 0x65, 1);
		if (!dodirectenter_fn) return this->error_out(__LINE__);
	}
	sdk::game::func::c_funcs::Instance().f_ConnectToGameServer = decltype(sdk::game::func::c_funcs::Instance().
		f_ConnectToGameServer)(dodirectenter_fn);

	sdk::util::c_log::Instance().duo(XorStr("[ off_directenter fn: %04x ]\n"), dodirectenter_fn);

	//

	auto CAccountConnector_SetLoginInfo = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetLoginInfo"));
	if (!CAccountConnector_SetLoginInfo) return this->error_out(__LINE__);

	sdk::util::c_log::Instance().duo(XorStr("[ off_SetLoginInfo: %04x ]\n"), CAccountConnector_SetLoginInfo);

	auto CAccountConnector_Set = sdk::util::c_disassembler::Instance().get_calls(CAccountConnector_SetLoginInfo, 0, 0);
	if (!CAccountConnector_Set.size()) return this->error_out(__LINE__);

	auto CAccountConnector_Set_off = sdk::util::c_disassembler::Instance().get_custom(
		CAccountConnector_Set[CAccountConnector_Set.size() - 2], 0, 0x50, 0xFF, {"mov", "add", "push", "lea"});
	if (CAccountConnector_Set_off.empty()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_USERNAME = CAccountConnector_Set_off.front();
	sdk::game::connection_offsets::off_PASSWORD = CAccountConnector_Set_off[1];
	if (CAccountConnector_Set_off.size() > 2) sdk::game::connection_offsets::off_PASSCODE = CAccountConnector_Set_off[2
	];

	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_accnet = decltype(sdk::game::func::c_funcs::Instance().
		f_SetLoginInfo_accnet)(CAccountConnector_Set[CAccountConnector_Set.size() - 2]);
	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_pynet = decltype(sdk::game::func::c_funcs::Instance().
		f_SetLoginInfo_pynet)(CAccountConnector_Set[CAccountConnector_Set.size() - 3]);
	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_accnet = decltype(sdk::game::func::c_funcs::Instance().
		f_SetLoginInfo_passcode_accnet)(CAccountConnector_Set[CAccountConnector_Set.size() - 2]);
	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_pynet = decltype(sdk::game::func::c_funcs::Instance().
		f_SetLoginInfo_passcode_accnet)(CAccountConnector_Set[CAccountConnector_Set.size() - 3]);

	sdk::util::c_log::Instance().duo(XorStr("[ f_SetLoginInfo_accnet: %04x ]\n"),
	                                 CAccountConnector_Set[CAccountConnector_Set.size() - 2]);
	sdk::util::c_log::Instance().duo(XorStr("[ f_SetLoginInfo_pynet : %04x ]\n"),
	                                 CAccountConnector_Set[CAccountConnector_Set.size() - 3]);

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("xaleas"))) //custom swapped params
	{
		sdk::game::connection_offsets::off_USERNAME = CAccountConnector_Set_off[1];
		sdk::game::connection_offsets::off_PASSWORD = CAccountConnector_Set_off[2];
		sdk::game::connection_offsets::off_PASSCODE = CAccountConnector_Set_off[0];

		sdk::util::c_log::Instance().duo(XorStr("[ xaleas off_USERNAME: %04x ]\n"),
		                                 sdk::game::connection_offsets::off_PASSWORD);
		sdk::util::c_log::Instance().duo(XorStr("[ xaleas off_PASSWORD: %04x ]\n"),
		                                 sdk::game::connection_offsets::off_PASSCODE);
		sdk::util::c_log::Instance().duo(XorStr("[ xaleas off_PASSCODE: %04x ]\n"),
		                                 sdk::game::connection_offsets::off_USERNAME);
	}
	else
	{
		sdk::util::c_log::Instance().duo(XorStr("[ off_USERNAME: %04x ]\n"),
		                                 sdk::game::connection_offsets::off_USERNAME);
		sdk::util::c_log::Instance().duo(XorStr("[ off_PASSWORD: %04x ]\n"),
		                                 sdk::game::connection_offsets::off_PASSWORD);
		if (!strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Zeros2")) if (CAccountConnector_Set_off.
			size() > 2) sdk::util::c_log::Instance().duo(XorStr("[ off_PASSCODE: %04x ]\n"),
			                                             sdk::game::connection_offsets::off_PASSCODE);
	}
	//

	auto CPythonNetworkStream_SetMarkServer = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetMarkServer"));
	if (!CPythonNetworkStream_SetMarkServer) return this->error_out(__LINE__);
	auto CPythonNetworkStream_SetMarkServer_calls = sdk::util::c_disassembler::Instance().get_calls(
		CPythonNetworkStream_SetMarkServer, 0, 0);
	if (CPythonNetworkStream_SetMarkServer_calls.empty())
	{
		CPythonNetworkStream_SetMarkServer_calls = sdk::util::c_disassembler::Instance().get_jumps(
			CPythonNetworkStream_SetMarkServer, 0, 0);
		if (CPythonNetworkStream_SetMarkServer_calls.empty()) return this->error_out(__LINE__);
	}

	auto CPythonNetworkStream_instance = this->find_singleton_or_instance(CPythonNetworkStream_SetMarkServer);
	if (!CPythonNetworkStream_instance) return this->error_out(__LINE__);

	sdk::game::pointer_offsets::off_CPythonNetworkStream = CPythonNetworkStream_instance;
	sdk::game::func::c_funcs::Instance().f_SetMarkServer = decltype(sdk::game::func::c_funcs::Instance().f_SetMarkServer
	)(CPythonNetworkStream_SetMarkServer_calls[CPythonNetworkStream_SetMarkServer_calls.size() - 2]);
	sdk::util::c_log::Instance().duo(XorStr("[ f_SetMarkServer: %04x ]\n"),
	                                 CPythonNetworkStream_SetMarkServer_calls[CPythonNetworkStream_SetMarkServer_calls.
		                                 size() - 2]);
	sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonNetworkStream: %04x ]\n"), CPythonNetworkStream_instance);

	//

	auto CPythonNetworkStream_SetOffLinePhase = sdk::util::c_fn_discover::Instance().get_fn(
		XorStr("## Network - OffLine Phase ##"));
	if (!CPythonNetworkStream_SetOffLinePhase)
	{
		//todo find good alt ref
		auto SetOfflinePhase_py = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetOfflinePhase"));
		if (!SetOfflinePhase_py) return this->error_out(__LINE__);

		auto SetOfflinePhase_py_calls = sdk::util::c_disassembler::Instance().get_calls(SetOfflinePhase_py, 0xB, 0, 0);
		if (SetOfflinePhase_py_calls.empty()) return this->error_out(__LINE__);

		for (auto b : SetOfflinePhase_py_calls) sdk::util::c_log::Instance().duo(XorStr("[ %04x ]\n"), b);

		if (SetOfflinePhase_py_calls.size() > 1) CPythonNetworkStream_SetOffLinePhase = SetOfflinePhase_py_calls[
			SetOfflinePhase_py_calls.size() - 2];
		else CPythonNetworkStream_SetOffLinePhase = SetOfflinePhase_py_calls.front();
		if (!CPythonNetworkStream_SetOffLinePhase) return this->error_out(__LINE__);
	}

	auto CPythonNetworkStream_SetOffLinePhase_off = sdk::util::c_disassembler::Instance().get_custom(
		CPythonNetworkStream_SetOffLinePhase, 0, 0x50, 0x1FF, {"mov", "add", "push", "lea"});
	if (CPythonNetworkStream_SetOffLinePhase_off.empty()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_NETWORKING_PHASE = CPythonNetworkStream_SetOffLinePhase_off.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_NETWORKING_PHASE: %04x ]\n"),
	                                 CPythonNetworkStream_SetOffLinePhase_off.front());

	//

	auto CNetworkStream_IsOnline = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("IsConnect"));
	if (!CNetworkStream_IsOnline) return this->error_out(__LINE__);

	auto CNetworkStream_IsOnline_ = sdk::util::c_fn_discover::Instance().discover_fn(CNetworkStream_IsOnline, 0x2, 0x4);
	if (!CNetworkStream_IsOnline_) return this->error_out(__LINE__);

	auto CNetworkStream_IsOnline_off = sdk::util::c_disassembler::Instance().get_custom(
		CNetworkStream_IsOnline_, 0, 0x20, 0xFF, {"mov", "add", "push", "lea"});
	if (CNetworkStream_IsOnline_off.empty()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_IS_CONNECTED = CNetworkStream_IsOnline_off.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_IS_CONNECTED: %04x ]\n"), CNetworkStream_IsOnline_off.front());

	//

	auto CPythonNetworkStream_SendSelectCharacterPacket = sdk::util::c_fn_discover::Instance().get_fn(
		XorStr("SendSelectCharacterPacket - Error"));
	if (!CPythonNetworkStream_SendSelectCharacterPacket) return this->error_out(__LINE__);

	sdk::game::func::c_funcs::Instance().f_SendSelectCharacter = decltype(sdk::game::func::c_funcs::Instance().
		f_SendSelectCharacter)(CPythonNetworkStream_SendSelectCharacterPacket);
	sdk::game::func::c_funcs::Instance().o_SendSelectCharacter = CPythonNetworkStream_SendSelectCharacterPacket;

	sdk::util::c_log::Instance().duo(XorStr("[ off_SendSelectCharacter: %04x ]\n"),
	                                 CPythonNetworkStream_SendSelectCharacterPacket);

	//

	//CPythonCharacterManager

	//

	auto SetPacketSequenceMode_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetPacketSequenceMode"));
	if (SetPacketSequenceMode_fn)
	{
		auto SetPacketSequenceMode_calls = sdk::util::c_fn_discover::Instance().discover_fn(
			SetPacketSequenceMode_fn, 0xA, 0xF, 0);
		if (SetPacketSequenceMode_calls)
		{
			auto SetPacketSequenceMode_off = sdk::util::c_disassembler::Instance().get_custom(
				SetPacketSequenceMode_calls, 0, 0x30, 0xFF, {"mov", "add", "push", "lea"});
			if (!SetPacketSequenceMode_off.empty())
			{
				sdk::game::connection_offsets::off_PACKET_SEQUENCE_MODE = SetPacketSequenceMode_off.front();
				sdk::util::c_log::Instance().duo(XorStr("[ off_PACKET_SEQUENCE_MODE: %04x ]\n"),
				                                 SetPacketSequenceMode_off.front());
			}
		}
	}

	return 1;
}

bool sdk::util::c_address_gathering::gather_actor_related()
{
	auto GetInstanceType_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("GetInstanceType"));
	if (!GetInstanceType_fn) return this->error_out(__LINE__);

	auto GetInstanceType_calls = sdk::util::c_disassembler::Instance().get_calls(GetInstanceType_fn, 0, 0, 1);
	if (GetInstanceType_calls.empty()) return this->error_out(__LINE__);

	auto CPythonCharacterManager_GetInstancePtr_fn = GetInstanceType_calls.back();
	auto CPythonCharacterManager_GetInstancePtr_off = sdk::util::c_disassembler::Instance().get_custom(
		GetInstanceType_calls.back(), 0, 0x100, 0x2000, {"mov", "add", "push", "lea"});

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Ekstasia"))
	{
		CPythonCharacterManager_GetInstancePtr_off = sdk::util::c_disassembler::Instance().get_custom(
			GetInstanceType_calls[GetInstanceType_calls.size() - 3], 0, 0x100, 0x2000, {"mov", "add", "push", "lea"});
		CPythonCharacterManager_GetInstancePtr_fn = GetInstanceType_calls[GetInstanceType_calls.size() - 3];
	}

	if (CPythonCharacterManager_GetInstancePtr_off.empty())
	{
		sdk::util::c_log::Instance().duo(XorStr("[ GetInstanceType_calls : %04x ]\n"), GetInstanceType_calls.back());
		CPythonCharacterManager_GetInstancePtr_off = sdk::util::c_disassembler::Instance().get_custom(
			GetInstanceType_calls[GetInstanceType_calls.size() - 3], 0, 0x50, 0x2000, {"mov", "add", "push", "lea"});
		CPythonCharacterManager_GetInstancePtr_fn = GetInstanceType_calls[GetInstanceType_calls.size() - 3];
		if (!CPythonCharacterManager_GetInstancePtr_off.size())
		{
			CPythonCharacterManager_GetInstancePtr_off = sdk::util::c_disassembler::Instance().get_custom(
				GetInstanceType_calls[GetInstanceType_calls.size() - 2], 0, 0x50, 0x2000,
				{"mov", "add", "push", "lea"});
			if (CPythonCharacterManager_GetInstancePtr_off.empty()) return this->error_out(__LINE__);
			CPythonCharacterManager_GetInstancePtr_fn = GetInstanceType_calls[GetInstanceType_calls.size() - 2];
		}
	}

	auto CInstanceBase_GetInstanceType_fn = sdk::util::c_disassembler::Instance().get_calls(
		CPythonCharacterManager_GetInstancePtr_fn, 0, 0, 0);
	if (CInstanceBase_GetInstanceType_fn.empty())
	{
		CInstanceBase_GetInstanceType_fn = sdk::util::c_disassembler::Instance().get_jumps(
			CPythonCharacterManager_GetInstancePtr_fn, 0, 0);
		if (CInstanceBase_GetInstanceType_fn.empty())
		{
			CInstanceBase_GetInstanceType_fn = sdk::util::c_disassembler::Instance().get_calls(
				CPythonCharacterManager_GetInstancePtr_fn, 0x16, 0, 0);
			if (CInstanceBase_GetInstanceType_fn.empty())
			{
				CInstanceBase_GetInstanceType_fn = sdk::util::c_disassembler::Instance().get_jumps(
					CPythonCharacterManager_GetInstancePtr_fn, 0x16, 0);
				if (CInstanceBase_GetInstanceType_fn.empty()) return this->error_out(__LINE__);
			}
		}
	}

	sdk::util::c_log::Instance().duo(XorStr("[ CInstanceBase_GetInstanceType_fn : %04x ]\n"),
	                                 CInstanceBase_GetInstanceType_fn.back());

	auto CInstanceBase_GetInstanceType_off = sdk::util::c_disassembler::Instance().get_custom(
		CInstanceBase_GetInstanceType_fn.back(), 0, 0x100, 0x2000, {"mov", "add", "push", "lea"});
	if (CInstanceBase_GetInstanceType_off.empty())
	{
		sdk::util::c_log::Instance().duo(XorStr("[ CInstanceBase_GetInstanceType_fn : %04x ]\n"),
		                                 CInstanceBase_GetInstanceType_fn.back());
		CInstanceBase_GetInstanceType_off = sdk::util::c_disassembler::Instance().get_custom(
			CInstanceBase_GetInstanceType_fn.back(), 0, 0x50, 0x2000, {"mov", "add", "push", "lea"});
		if (CInstanceBase_GetInstanceType_off.empty()) return this->error_out(__LINE__);
	}

	sdk::game::actor_offsets::off_GRAPHIC_THING = CPythonCharacterManager_GetInstancePtr_off.front();
	sdk::game::actor_offsets::off_ACTOR_TYPE = CInstanceBase_GetInstanceType_off.front();
	sdk::game::actor_offsets::off_COMBO_INDEX = CInstanceBase_GetInstanceType_off.front() - 0x4;
	sdk::util::c_log::Instance().duo(XorStr("[ off_GRAPHIC_THING : %04x ]\n"),
	                                 CPythonCharacterManager_GetInstancePtr_off.front());
	sdk::util::c_log::Instance().duo(XorStr("[ off_ACTOR_TYPE    : %04x ]\n"),
	                                 CInstanceBase_GetInstanceType_off.front());
	sdk::util::c_log::Instance().duo(XorStr("[ off_COMBO_INDEX    : %04x ]\n"),
	                                 CInstanceBase_GetInstanceType_off.front() - 0x4);

	//

	auto CPythonPlayer_OpenCharacterMenu = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("OpenCharacterMenu"));
	if (!CPythonPlayer_OpenCharacterMenu) return this->error_out(__LINE__);

	auto possible_NEW_GetMainActorPtr = sdk::util::c_fn_discover::Instance().get_adr_str(XorStr("SetPCTargetBoard"), 2);
	if (possible_NEW_GetMainActorPtr.empty())
	{
		possible_NEW_GetMainActorPtr = sdk::util::c_fn_discover::Instance().get_adr_str(XorStr("SetPCTargetBoard"), 6);
		//singletona assert
		if (possible_NEW_GetMainActorPtr.empty())
		{
			possible_NEW_GetMainActorPtr = sdk::util::c_fn_discover::Instance().get_adr_str(XorStr("SetPCTargetBoard"),
				4); //singletona assert
			if (possible_NEW_GetMainActorPtr.empty()) return this->error_out(__LINE__);
		}
	}

	auto CPythonPlayer_NEW_GetMainActorPtr_off = sdk::util::c_disassembler::Instance().get_custom(
		possible_NEW_GetMainActorPtr.front(), 0, 0x20, 0x100, {"call", "mov"});
	if (CPythonPlayer_NEW_GetMainActorPtr_off.empty()) return this->error_out(__LINE__);

	auto CPythonPlayer_instance = this->find_singleton_or_instance(possible_NEW_GetMainActorPtr.front());
	if (!CPythonPlayer_instance) return this->error_out(__LINE__);


	sdk::game::pointer_offsets::off_CPythonPlayer = CPythonPlayer_instance;
	sdk::game::actor_offsets::off_VTABLE_GET_MAIN = CPythonPlayer_NEW_GetMainActorPtr_off.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_VTABLE_GET_MAIN    : %04x ]\n"),
	                                 CPythonPlayer_NEW_GetMainActorPtr_off.front());
	sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonPlayer      : %04x ]\n"), CPythonPlayer_instance);

	//

	auto SetVirtualID_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetVirtualID"));
	if (!SetVirtualID_fn) return this->error_out(__LINE__);

	auto SetVirtualID_calls = sdk::util::c_disassembler::Instance().get_calls(SetVirtualID_fn, 0, 0, 1);
	if (SetVirtualID_calls.empty()) return this->error_out(__LINE__);

	auto CInstanceBase_SetVirtualID = sdk::util::c_disassembler::Instance().get_calls(
		SetVirtualID_calls[SetVirtualID_calls.size() - 2], 0, 0, 1);
	if (CInstanceBase_SetVirtualID.empty())
	{
		CInstanceBase_SetVirtualID = sdk::util::c_disassembler::Instance().get_jumps(
			SetVirtualID_calls[SetVirtualID_calls.size() - 2], 0, 0);
		if (CInstanceBase_SetVirtualID.empty()) return this->error_out(__LINE__);
	}

	auto SetVirtualID_offs = sdk::util::c_disassembler::Instance().get_custom(
		CInstanceBase_SetVirtualID.front(), 0, 0x200, 0x2000, {"mov", "push"});
	if (SetVirtualID_offs.empty()) return this->error_out(__LINE__);

	sdk::game::actor_offsets::off_VIRTUAL_ID = SetVirtualID_offs.front();
	sdk::game::actor_offsets::off_SHOULD_SKIP_COLLISION = SetVirtualID_offs.front() + 0x8;
	sdk::util::c_log::Instance().duo(XorStr("[ off_VIRTUAL_ID: %04x ]\n"), SetVirtualID_offs.front());
	sdk::util::c_log::Instance().duo(XorStr("[ off_SHOULD_SKIP_COLLISION: %04x ]\n"), SetVirtualID_offs.front() + 0x8);

	//

	auto SetNameString_fn = sdk::util::c_fn_discover::Instance().get_fn_py("SetNameString");
	if (!SetNameString_fn) return this->error_out(__LINE__);

	auto SetNameString_calls = sdk::util::c_disassembler::Instance().get_calls(SetNameString_fn, 0, 0, 1);
	if (SetNameString_calls.empty()) return this->error_out(__LINE__);

	auto CInstanceBase_SetNameString = sdk::util::c_disassembler::Instance().get_custom(
		SetNameString_calls[SetNameString_calls.size() - 2], 0, 0x8, 0x30, {"add", "lea"});
	if (CInstanceBase_SetNameString.empty())
	{
		CInstanceBase_SetNameString = sdk::util::c_disassembler::Instance().get_custom(
			SetNameString_calls[SetNameString_calls.size() - 2], 0, 0x30, 0x65, {"add", "lea"});
		if (CInstanceBase_SetNameString.empty()) return this->error_out(__LINE__);
	}

	sdk::game::actor_offsets::off_NAME = CInstanceBase_SetNameString.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_NAME: %04x ]\n"), CInstanceBase_SetNameString.front());

	//

	auto GetPixelPosition_fn = sdk::util::c_fn_discover::Instance().get_fn_py("GetPixelPosition");
	if (!GetPixelPosition_fn) return this->error_out(__LINE__);

	auto CInstanceBase_NEW_GetPixelPosition = sdk::util::c_fn_discover::Instance().discover_fn(
		GetPixelPosition_fn, 0x20, 0x39, 1);
	if (!CInstanceBase_NEW_GetPixelPosition) return this->error_out(__LINE__);

	auto CInstanceBase_NEW_GetPixelPosition_calls = sdk::util::c_disassembler::Instance().get_calls(
		CInstanceBase_NEW_GetPixelPosition, 0, 0, 1);
	if (CInstanceBase_NEW_GetPixelPosition_calls.empty())
	{
		CInstanceBase_NEW_GetPixelPosition_calls = sdk::util::c_disassembler::Instance().get_jumps(
			CInstanceBase_NEW_GetPixelPosition, 0, 0);
		if (CInstanceBase_NEW_GetPixelPosition_calls.empty()) return this->error_out(__LINE__);
	}

	sdk::util::c_log::Instance().duo(XorStr("[ CInstanceBase_NEW_GetPixelPosition_calls: %04x ]\n"),
	                                 CInstanceBase_NEW_GetPixelPosition_calls.back());

	auto CInstanceBase_NEW_GetPixelPosition_off = sdk::util::c_disassembler::Instance().get_custom(
		CInstanceBase_NEW_GetPixelPosition_calls.front(), 0, 0x200, 0x3000, {"mov"});
	if (CInstanceBase_NEW_GetPixelPosition_off.empty())
	{
		CInstanceBase_NEW_GetPixelPosition_off = sdk::util::c_disassembler::Instance().get_custom(
			CInstanceBase_NEW_GetPixelPosition_calls.back(), 0, 0x200, 0x3000, {"mov"});
		if (CInstanceBase_NEW_GetPixelPosition_off.empty()) return this->error_out(__LINE__);
	}

	sdk::game::actor_offsets::off_POSITION = CInstanceBase_NEW_GetPixelPosition_off.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_POSITION: %04x ]\n"),
	                                 CInstanceBase_NEW_GetPixelPosition_off.front());

	//

	auto CPythonNetworkStream_SendCharacterStatePacket = sdk::util::c_fn_discover::Instance().get_fn(
		"SendCharacterStatePacket(dwCmdTime=%u");
	if (!CPythonNetworkStream_SendCharacterStatePacket)
	{
		auto CPyhtonNetworkStream_SendAttack = sdk::util::c_fn_discover::Instance().get_fn(
			"Send Battle Attack Packet Error");
		if (!CPyhtonNetworkStream_SendAttack) return this->error_out(__LINE__);

		sdk::game::func::c_funcs::Instance().o_SendCharacterStatePacket = CPythonNetworkStream_SendCharacterStatePacket;
		sdk::game::func::c_funcs::Instance().f_SendCharacterStatePacket = decltype(sdk::game::func::c_funcs::Instance().
			f_SendCharacterStatePacket)(
			CPyhtonNetworkStream_SendAttack + sdk::util::c_mem::Instance().find_size(CPyhtonNetworkStream_SendAttack) +
			0x10);
		CPythonNetworkStream_SendCharacterStatePacket = CPyhtonNetworkStream_SendAttack + sdk::util::c_mem::Instance().
			find_size(CPyhtonNetworkStream_SendAttack) + 0x10;
	}
	else
	{
		sdk::game::func::c_funcs::Instance().f_SendCharacterStatePacket = decltype(sdk::game::func::c_funcs::Instance().
			f_SendCharacterStatePacket)(CPythonNetworkStream_SendCharacterStatePacket);
		sdk::game::func::c_funcs::Instance().o_SendCharacterStatePacket = CPythonNetworkStream_SendCharacterStatePacket;
	}

	sdk::util::c_log::Instance().duo(XorStr("[ f_SendCharacterStatePacket: %04x ]\n"),
	                                 CPythonNetworkStream_SendCharacterStatePacket);

	//
	auto use_special_fn = 0;

	auto CPyhtonNetworkStream_SendAttack = sdk::util::c_fn_discover::Instance().get_fn(
		XorStr("Send Battle Attack Packet Error"));
	if (!CPyhtonNetworkStream_SendAttack)
	{
		//alt through ref
		auto CPythonPlayerEventHandler_CNormalBowAttack_FlyEventHandler_AutoClear_OnExplodingAtAnotherTarget =
			sdk::util::c_fn_discover::Instance().get_fn(XorStr("Shoot : Hitting another target : %"));
		if (!CPythonPlayerEventHandler_CNormalBowAttack_FlyEventHandler_AutoClear_OnExplodingAtAnotherTarget)
		{
			CPythonPlayerEventHandler_CNormalBowAttack_FlyEventHandler_AutoClear_OnExplodingAtAnotherTarget =
				sdk::util::c_fn_discover::Instance().get_fn(XorStr("CPythonPlayerEventHandler::OnHit"));
			if (!CPythonPlayerEventHandler_CNormalBowAttack_FlyEventHandler_AutoClear_OnExplodingAtAnotherTarget) return
				this->error_out(__LINE__);
			use_special_fn = 1;
		}

		CPyhtonNetworkStream_SendAttack = sdk::util::c_fn_discover::Instance().discover_fn(
			CPythonPlayerEventHandler_CNormalBowAttack_FlyEventHandler_AutoClear_OnExplodingAtAnotherTarget, 0x50, 0x80,
			3, 0, 0, 1);
		if (use_special_fn)
		{
			CPyhtonNetworkStream_SendAttack =
				CPythonPlayerEventHandler_CNormalBowAttack_FlyEventHandler_AutoClear_OnExplodingAtAnotherTarget;
			sdk::game::func::c_funcs::Instance().o_SendHitEvent = CPyhtonNetworkStream_SendAttack;
			sdk::game::func::c_funcs::Instance().f_OnHit = decltype(sdk::game::func::c_funcs::Instance().f_OnHit)(
				CPyhtonNetworkStream_SendAttack);
			auto CPythonPlayerEventHandler_instance = this->find_singleton_or_instance(
				sdk::util::c_fn_discover::Instance().get_fn(XorStr("RefreshStamina")));
			if (!CPythonPlayerEventHandler_instance) return this->error_out(__LINE__);
			sdk::game::pointer_offsets::off_CPythonPlayerEventHandler = CPythonPlayerEventHandler_instance;
		}
	}

	if (!use_special_fn)
	{
		sdk::game::func::c_funcs::Instance().f_SendAttackPacket = decltype(sdk::game::func::c_funcs::Instance().
			f_SendAttackPacket)(CPyhtonNetworkStream_SendAttack);
		sdk::util::c_log::Instance().duo(XorStr("[ f_SendAttackPacket: %04x ]\n"), CPyhtonNetworkStream_SendAttack);
	}
	else
	{
		sdk::util::c_log::Instance().duo(XorStr("[ o_SendHitEvent: %04x ]\n"),
		                                 sdk::game::func::c_funcs::Instance().o_SendHitEvent);
		sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonPlayerEventHandler_instance: %04x ]\n"),
		                                 sdk::game::pointer_offsets::off_CPythonPlayerEventHandler);
	}

	//
	bool use_fixed_size = 0;
	auto CPythonNetworkStream_SendItemUsePacket = sdk::util::c_fn_discover::Instance().get_fn(
		(XorStr("SendItemMovePacket Error")));
	if (!CPythonNetworkStream_SendItemUsePacket)
	{
		CPythonNetworkStream_SendItemUsePacket = sdk::util::c_fn_discover::Instance().get_fn(
			(XorStr("CANNOT_EQUIP_EXCHANGE")));
		if (!CPythonNetworkStream_SendItemUsePacket)
		{
			CPythonNetworkStream_SendItemUsePacket = sdk::util::c_fn_discover::Instance().get_fn_py(
				(XorStr("SendItemMovePacket")));
			if (!CPythonNetworkStream_SendItemUsePacket) return this->error_out(__LINE__);

			auto CPythonNetworkStream_SendItemUsePacket_calls = sdk::util::c_disassembler::Instance().get_calls(
				CPythonNetworkStream_SendItemUsePacket, 0, 0, 1);

			sdk::util::c_log::Instance().duo("[ last fn in senditemusepacket: %04x ]\n",
			                                 CPythonNetworkStream_SendItemUsePacket_calls[
				                                 CPythonNetworkStream_SendItemUsePacket_calls.size() - 2]);

			CPythonNetworkStream_SendItemUsePacket = CPythonNetworkStream_SendItemUsePacket_calls[
				CPythonNetworkStream_SendItemUsePacket_calls.size() - 2];

			use_fixed_size = 1;
		}
	}

	auto addor = 0x0;
	if (use_fixed_size) addor = 0x111;

	auto CPythonNetworkStream_IsPlayerAttacking = sdk::util::c_fn_discover::Instance().discover_fn(
		CPythonNetworkStream_SendItemUsePacket, 0x1A, 0x20, 2, 0, 0, 0, 1, 0, 0, 0, addor);
	if (!CPythonNetworkStream_IsPlayerAttacking)
	{
		CPythonNetworkStream_IsPlayerAttacking = sdk::util::c_fn_discover::Instance().discover_fn(
			CPythonNetworkStream_SendItemUsePacket, 0x3A, 0x3F, 2, 0, 0, 0, 1, 0, 0, 0, addor);
		if (!CPythonNetworkStream_IsPlayerAttacking) return this->error_out(__LINE__);
	}

	auto CPythonNetworkStream_IsPlayerAttacking_calls = sdk::util::c_disassembler::Instance().get_calls(
		CPythonNetworkStream_IsPlayerAttacking, 0, 0, 1);
	if (CPythonNetworkStream_IsPlayerAttacking_calls.empty()) return this->error_out(__LINE__);

	sdk::game::func::c_funcs::Instance().f_IsAttacking = decltype(sdk::game::func::c_funcs::Instance().f_IsAttacking)(
		CPythonNetworkStream_IsPlayerAttacking_calls.back());
	sdk::game::func::c_funcs::Instance().o_IsAttacking = CPythonNetworkStream_IsPlayerAttacking_calls.back();
	sdk::util::c_log::Instance().duo(XorStr("[ f_IsAttacking: %04x ]\n"),
	                                 CPythonNetworkStream_IsPlayerAttacking_calls.back());

	//

	auto CPythonCharacterManager_FaintTest = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("FaintTest"));
	if (!CPythonCharacterManager_FaintTest) this->error_out(__LINE__);

	auto CPythonCharacterManager_instance = this->find_singleton_or_instance(CPythonCharacterManager_FaintTest);
	if (!CPythonCharacterManager_instance) this->error_out(__LINE__);

	auto IsFaint_fn = sdk::util::c_fn_discover::Instance().discover_fn(CPythonCharacterManager_FaintTest, 0xC, 0xE, 0,
	                                                                   0, true, true, 1, 0, 0,
	                                                                   sdk::game::actor_offsets::off_COMBO_INDEX);
	if (!IsFaint_fn)
	{
		IsFaint_fn = sdk::util::c_fn_discover::Instance().discover_fn(CPythonCharacterManager_FaintTest, 0x8, 0x12, 0,
		                                                              0, true, true, 1, 0, 0,
		                                                              sdk::game::actor_offsets::off_COMBO_INDEX);
		if (!IsFaint_fn)
		{
			auto IsFaint_calls = sdk::util::c_disassembler::Instance().get_calls(
				CPythonCharacterManager_FaintTest, 0, 0, 1);
			if (IsFaint_calls.empty()) return this->error_out(__LINE__);
			IsFaint_fn = IsFaint_calls[IsFaint_calls.size() - 2];
			if (!IsFaint_fn) this->error_out(__LINE__);

			if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Tamidia")) IsFaint_fn = IsFaint_calls[
				2];
		}
	}

	sdk::util::c_log::Instance().duo(XorStr("[ IsFaint_fn: %04x ]\n"), IsFaint_fn);

	auto IsFaint_off = sdk::util::c_disassembler::Instance().get_custom(IsFaint_fn, 0, 0x300, 0x2000,
	                                                                    {"mov", "lea", "push"});
	if (IsFaint_off.empty()) return this->error_out(__LINE__);

	sdk::game::actor_offsets::off_FAINT = IsFaint_off.front();
	sdk::game::actor_offsets::off_SLEEP = IsFaint_off.front() - 0x4;
	sdk::game::actor_offsets::off_STUN = IsFaint_off.front() + 0x8;
	sdk::game::actor_offsets::off_DEAD = IsFaint_off.front() + 0xC;
	sdk::game::pointer_offsets::off_CPythonCharacterManager = CPythonCharacterManager_instance;

	sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonCharacterManager: %04x ]\n"),
	                                 CPythonCharacterManager_instance);
	sdk::util::c_log::Instance().duo(XorStr("[ off_FAINT: %04x ]\n"), IsFaint_off.front());
	sdk::util::c_log::Instance().duo(XorStr("[ off_SLEEP: %04x ]\n"), IsFaint_off.front() - 0x4);
	sdk::util::c_log::Instance().duo(XorStr("[ off_STUN : %04x ]\n"), IsFaint_off.front() + 0x8);
	sdk::util::c_log::Instance().duo(XorStr("[ off_DEAD : %04x ]\n"), IsFaint_off.front() + 0xC);

	//

	auto SendShootPacket = sdk::util::c_fn_discover::Instance().get_fn(XorStr("SendShootPacket Error"));
	if (!SendShootPacket) return this->error_out(__LINE__);

	sdk::game::func::c_funcs::Instance().f_SendShootPacket = decltype(sdk::game::func::c_funcs::Instance().f_SendShootPacket)(SendShootPacket);
	sdk::game::func::c_funcs::Instance().o_SendShootPacket = SendShootPacket ;
	
	sdk::util::c_log::Instance().duo(XorStr("[ f_SendShootPacket: %04x ]\n"), SendShootPacket );

	////

	auto SendFlyTargetingPacket = sdk::util::c_fn_discover::Instance().get_fn(XorStr("Send FlyTargeting Packet Error"));
	if (!SendFlyTargetingPacket) return this->error_out(__LINE__);

	sdk::game::func::c_funcs::Instance().f_SendFlyTargetingPacket = decltype(sdk::game::func::c_funcs::Instance().f_SendFlyTargetingPacket)(SendFlyTargetingPacket);
	
	sdk::util::c_log::Instance().duo(XorStr("[ f_SendFlyTargetingPacket: %04x ]\n"), SendFlyTargetingPacket);

	return 1;
}

bool sdk::util::c_address_gathering::gather_item_related()
{
	auto CPythonPlayer_SendClickItemPacket = sdk::util::c_fn_discover::Instance().get_fn(
		XorStr("CPythonPlayer::SendClickItemPacket"));
	if (!CPythonPlayer_SendClickItemPacket)
	{
		CPythonPlayer_SendClickItemPacket = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("PickCloseItem"));
		if (!CPythonPlayer_SendClickItemPacket) return this->error_out(__LINE__);
		CPythonPlayer_SendClickItemPacket = sdk::util::c_fn_discover::Instance().discover_fn(
			CPythonPlayer_SendClickItemPacket, 0x100, 0x160, 10);
		if (!CPythonPlayer_SendClickItemPacket) return this->error_out(__LINE__);
	}

	auto CPythonItem_GetOwnership = sdk::util::c_fn_discover::Instance().discover_fn(
		CPythonPlayer_SendClickItemPacket, 0x50, 0x70, 0, 0, 0, 1);
	if (!CPythonItem_GetOwnership)
	{
		CPythonItem_GetOwnership = sdk::util::c_fn_discover::Instance().discover_fn(
			CPythonPlayer_SendClickItemPacket, 0x50, 0x70, 3, 0, 0, 1, 1);
		if (!CPythonItem_GetOwnership) return this->error_out(__LINE__);
	}

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Celestial World 2.0") ||
		strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Yumano3"))
		CPythonItem_GetOwnership = sdk::util::c_fn_discover::Instance().discover_fn(
			CPythonPlayer_SendClickItemPacket, 0x50, 0x70, 0, 0, 0, 1);

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Hybrid2"))
	{
		CPythonPlayer_SendClickItemPacket = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("PickCloseItem"));
		if (!CPythonPlayer_SendClickItemPacket) return this->error_out(__LINE__);
		auto SendClickItemPacket_calls = sdk::util::c_disassembler::Instance().get_calls(
			CPythonPlayer_SendClickItemPacket, 0, 0, 0);
		if (SendClickItemPacket_calls.empty()) return this->error_out(__LINE__);
		CPythonPlayer_SendClickItemPacket = SendClickItemPacket_calls[SendClickItemPacket_calls.size() - 2];
		SendClickItemPacket_calls = sdk::util::c_disassembler::Instance().get_calls(
			CPythonPlayer_SendClickItemPacket, 0, 0, 0);
		CPythonPlayer_SendClickItemPacket = SendClickItemPacket_calls[SendClickItemPacket_calls.size() - 2];
		sdk::util::c_log::Instance().duo(XorStr("[ CPythonPlayer_SendClickItemPacket mfn: %04x ]\n"),
		                                 CPythonPlayer_SendClickItemPacket);
	}

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Hybrid2")) CPythonItem_GetOwnership =
		sdk::util::c_fn_discover::Instance().discover_fn(CPythonPlayer_SendClickItemPacket, 0x76, 0x78, 0, 0, 0, 1);

	sdk::util::c_log::Instance().duo(XorStr("[ CPythonItem_GetOwnership: %04x ]\n"), CPythonItem_GetOwnership);

	auto GetOwnership_off = sdk::util::c_disassembler::Instance().get_custom(
		CPythonItem_GetOwnership, 0, 0x100, 0x500, {"mov", "lea", "add"});
	if (GetOwnership_off.empty()) return this->error_out(__LINE__);

	sdk::game::item_offsets::off_OWNER_NAME = GetOwnership_off.back();
	sdk::util::c_log::Instance().duo(XorStr("[ off_OWNER_NAME: %04x ]\n"), GetOwnership_off.back());

	//

	auto SendItemPickUpPacket = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SendItemPickUpPacket"));

	//untested
	auto CPythonNetworkStream_SendItemPickUpPacket_list = sdk::util::c_fn_discover::Instance().get_adr_str(
		XorStr("SendItemPickUpPacket Error"), 1);
	if (CPythonNetworkStream_SendItemPickUpPacket_list.empty()) return this->error_out(__LINE__);
	auto CPythonNetworkStream_SendItemPickUpPacket = CPythonNetworkStream_SendItemPickUpPacket_list.front();
	if (CPythonNetworkStream_SendItemPickUpPacket)
	{
		//use str ref
		sdk::util::c_log::Instance().duo(XorStr("[ R1 f_SendItemPickUpPacket: %04x ]\n"),
		                                 CPythonNetworkStream_SendItemPickUpPacket);
		sdk::game::func::c_funcs::Instance().f_SendItemPickUpPacket = decltype(sdk::game::func::c_funcs::Instance().
			f_SendItemPickUpPacket)(CPythonNetworkStream_SendItemPickUpPacket);
		auto SendClickItemPacket = sdk::util::c_fn_discover::Instance().get_fn(XorStr("OnCannotPickItem"));
		if (!SendClickItemPacket) SendClickItemPacket = sdk::util::c_fn_discover::Instance().get_fn(
			XorStr("CPythonPlayer::SendClickItemPacket(dwIID"));
		if (!SendClickItemPacket) return this->error_out(__LINE__);
		sdk::util::c_log::Instance().duo(XorStr("[ SendClickItemPacket: %04x ]\n"), SendClickItemPacket);
		sdk::game::pointer_offsets::off_CPythonItem = this->find_singleton_or_instance(SendClickItemPacket);
		if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("xaleas")))
		{
			auto f = sdk::util::c_fn_discover::Instance().get_fn(
				XorStr("CPythonPlayer::SendClickItemPacket(dwIID=%d) : Non-exist item"));
			auto f_movs = sdk::util::c_disassembler::Instance().get_custom(f, 0, 0, 0, {"mov"});
			sdk::game::pointer_offsets::off_CPythonItem = f_movs.front();
		}
		if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Zeros2")))
		{
			auto f = sdk::util::c_fn_discover::Instance().get_fn(XorStr("CPythonPlayer::__OnPressItem - ALREADY"));
			auto f_movs = sdk::util::c_disassembler::Instance().get_custom(f, 0, 0, 0, {"mov"});
			sdk::game::pointer_offsets::off_CPythonItem = f_movs.front();
		}
	}
	else
	{
		//use pyfunc method
		sdk::util::c_log::Instance().duo(XorStr("[ R2 f_SendItemPickUpPacket: %04x ]\n"),
		                                 CPythonNetworkStream_SendItemPickUpPacket - (uint32_t)GetModuleHandleA(0) +
		                                 0x400000);
		if (!SendItemPickUpPacket) return this->error_out(__LINE__);
		CPythonNetworkStream_SendItemPickUpPacket = sdk::util::c_fn_discover::Instance().discover_fn(
			SendItemPickUpPacket, 0x50, 0x70, 3);
		if (!CPythonNetworkStream_SendItemPickUpPacket) return this->error_out(__LINE__);
		sdk::game::func::c_funcs::Instance().f_SendItemPickUpPacket = decltype(sdk::game::func::c_funcs::Instance().
			f_SendItemPickUpPacket)(CPythonNetworkStream_SendItemPickUpPacket);
		sdk::game::pointer_offsets::off_CPythonItem = this->find_singleton_or_instance(SendItemPickUpPacket);
	}
	// ReSharper disable once CppCStyleCast
	sdk::util::c_log::Instance().duo(XorStr("[ f_SendItemPickUpPacket: %04x ]\n"),
	                                 CPythonNetworkStream_SendItemPickUpPacket - (uint32_t)GetModuleHandleA(0));
	sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonItem: %04x ]\n"),
	                                 sdk::game::pointer_offsets::off_CPythonItem);

	//

	auto GetItemName = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("GetItemName"));
	if (!GetItemName) return this->error_out(__LINE__);

	auto GetItemName_calls = sdk::util::c_disassembler::Instance().get_calls(GetItemName, 0, 0, 1);
	if (GetItemName_calls.empty()) return this->error_out(__LINE__);

	auto GetItemName_name_off = sdk::util::c_disassembler::Instance().get_custom(
		GetItemName_calls.back(), 0, 0x50, 0x1000, {"mov", "lea", "add"});
	if (GetItemName_name_off.empty())
	{
		GetItemName_name_off = sdk::util::c_disassembler::Instance().get_custom(
			GetItemName_calls[GetItemName_calls.size() - 2], 0, 0x50, 0x1000, {"mov", "lea", "add"});
		if (GetItemName_name_off.empty())
		{
			GetItemName_name_off = sdk::util::c_disassembler::Instance().get_custom(
				GetItemName_calls[GetItemName_calls.size() - 3], 0, 0x50, 0x1000, {"mov", "lea", "add"});
			if (GetItemName_name_off.empty()) return this->error_out(__LINE__);
		}
	}

	auto CItemManager_Instance = this->find_singleton_or_instance(GetItemName);
	if (!CItemManager_Instance) return this->error_out(__LINE__);

	//if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Ekstasia"))) GetItemName_name_off[0] += 0x4;//shitty client
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("SunshineMt2"))) GetItemName_name_off[0]
		= 0xED; //shitty client

	sdk::game::item_offsets::off_ITEM_NAME = GetItemName_name_off.front();
	sdk::game::pointer_offsets::off_CItemManager = CItemManager_Instance;

	sdk::util::c_log::Instance().duo(XorStr("[ off_ITEM_NAME: %04x ]\n"), GetItemName_name_off.front());
	sdk::util::c_log::Instance().duo(XorStr("[ off_CItemManager: %04x ]\n"), CItemManager_Instance);

	return 1;
}

bool sdk::util::c_address_gathering::gather_background_related()
{
	auto GetCurrentMapName = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("GetCurrentMapName"));
	if (!GetCurrentMapName) return this->error_out(__LINE__);

	sdk::game::pointer_offsets::off_CPythonBackgroundManager = this->find_singleton_or_instance(GetCurrentMapName);
	if (!sdk::game::pointer_offsets::off_CPythonBackgroundManager) return this->error_out(__LINE__);

	auto GetCurrentMapName_getter = sdk::util::c_fn_discover::Instance().discover_fn(GetCurrentMapName, 0x14, 0x20, 1);
	if (!GetCurrentMapName_getter)
	{
		auto GetCurrentMapName_calls = sdk::util::c_disassembler::Instance().get_calls(GetCurrentMapName, 0, 0, 1);
		if (!GetCurrentMapName_calls.size()) return this->error_out(__LINE__);
		GetCurrentMapName_getter = GetCurrentMapName_calls.front();
		if (!GetCurrentMapName_getter) return this->error_out(__LINE__);
	}

	auto m_strMapName = sdk::util::c_disassembler::Instance().get_custom(
		GetCurrentMapName_getter, 0, 0x1000, 0x3000, {"mov", "push", "add", "lea"});
	if (m_strMapName.empty())
	{
		m_strMapName = sdk::util::c_disassembler::Instance().get_custom(GetCurrentMapName_getter, 0, 0xD0, 0x3000,
		                                                                {"mov", "push", "add", "lea"});
		if (m_strMapName.empty()) return this->error_out(__LINE__);
	}

	sdk::game::background::off_CUR_MAP_NAME = m_strMapName.front();
	sdk::game::pointer_offsets::off_CPythonBackgroundManager = this->find_singleton_or_instance(GetCurrentMapName);

	sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonBackgroundManager: %04x ]\n"),
	                                 this->find_singleton_or_instance(GetCurrentMapName));
	sdk::util::c_log::Instance().duo(XorStr("[ off_CUR_MAP_NAME: %04x ]\n"), m_strMapName.front());

	//

	auto MapLoadAtlas = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("LoadAtlas"));
	if (!MapLoadAtlas) return this->error_out(__LINE__);

	auto CPythonMiniMap_Instance = this->find_singleton_or_instance(MapLoadAtlas);
	if (!CPythonMiniMap_Instance) return this->error_out(__LINE__);

	/*auto MapLoadAtlas_func = sdk::util::c_fn_discover::Instance().discover_fn(MapLoadAtlas, 0x200, 0x300);
	if (!MapLoadAtlas_func) return this->error_out(__LINE__);*/

	sdk::game::pointer_offsets::off_CPythonMiniMap = CPythonMiniMap_Instance;
	sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonMiniMap: %04x ]\n"), CPythonMiniMap_Instance);

	//

	/*auto ShowAtlas = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("ShowAtlas"));
	if (!ShowAtlas) return this->error_out(__LINE__);

	auto ShowAtlas_func = sdk::util::c_fn_discover::Instance().discover_fn(ShowAtlas, 0x10, 0x18, 0, 0, true);
	if (!ShowAtlas_func) return this->error_out(__LINE__);

	auto ShowAtlas_off = sdk::util::c_disassembler::Instance().get_custom(
		ShowAtlas_func, 0, 0x100, 0x400, {"push", "mov", "add"});
	if (ShowAtlas_off.empty()) return this->error_out(__LINE__);

	sdk::game::minimap_offsets::off_SHOW_ATLAS = ShowAtlas_off.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_SHOW_ATLAS: %04x ]\n"), ShowAtlas_off.front());*/

	return 1;
}

bool sdk::util::c_address_gathering::gather_pack_related()
{
	auto RunPythonFile = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("RunPythonFile"));
	if (!RunPythonFile) return this->error_out(__LINE__);

	auto CPythonLauncher_RunFile = sdk::util::c_fn_discover::Instance().discover_fn(RunPythonFile, 0xE0, 0xF0);
	if (!CPythonLauncher_RunFile)
	{
		CPythonLauncher_RunFile = sdk::util::c_fn_discover::Instance().discover_fn(RunPythonFile, 0x130, 0x160);
		if (!CPythonLauncher_RunFile) return this->error_out(__LINE__);
	}

	auto CEterPackManager_Instance = 0;
	auto CPythonLauncher_RunFile_offs = sdk::util::c_disassembler::Instance().get_custom(CPythonLauncher_RunFile, 0, 0, 0, {"mov"});
	if (CPythonLauncher_RunFile_offs.empty()) return this->error_out(__LINE__);
	
	CEterPackManager_Instance = CPythonLauncher_RunFile_offs[1];

	sdk::game::pointer_offsets::off_CEterPackManager = CEterPackManager_Instance;
	sdk::util::c_log::Instance().duo(XorStr("[ off_CEterPackManager %04x ]\n"), CEterPackManager_Instance);

	auto CEterPackManager_Get = sdk::util::c_fn_discover::Instance().discover_fn(CPythonLauncher_RunFile, 0x200, 0x500);

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Hybrid2"))
	{
		CEterPackManager_Get = sdk::util::c_fn_discover::Instance().discover_fn(CPythonLauncher_RunFile, 0x7E, 0x89, 0, 0, 0, 1);
	}
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Metin2 SG"))
	{
		CEterPackManager_Get = sdk::util::c_fn_discover::Instance().discover_fn(CPythonLauncher_RunFile, 0xE0, 0xF0);

	}
	if (!CEterPackManager_Get) return this->error_out(__LINE__);
	
	sdk::util::c_log::Instance().duo(XorStr("[ f_EterPackGet %04x ]\n"), CEterPackManager_Get);

	auto CMappedFile_Size = sdk::util::c_fn_discover::Instance().discover_fn(CPythonLauncher_RunFile, 0x4, 0x8, 0, 0, true, true);
	if (!CMappedFile_Size) return this->error_out(__LINE__);

	auto CMappedFile_Size_off = sdk::util::c_disassembler::Instance().get_custom(CMappedFile_Size, 0, 0x50, 0x200, { "mov", "add", "lea" });
	if (CMappedFile_Size_off.empty()) return this->error_out(__LINE__);

	sdk::game::func::c_funcs::Instance().f_EterPackGet = decltype(sdk::game::func::c_funcs::Instance().f_EterPackGet)(CEterPackManager_Get);
	sdk::game::pack_offsets::off_CEterPackManager_GET = CEterPackManager_Get;
	sdk::game::pack_offsets::off_CMAPPED_FILE_SIZE = CMappedFile_Size_off.front();
	
	sdk::util::c_log::Instance().duo(XorStr("[ off_CMAPPED_FILE_SIZE %04x ]\n"), CMappedFile_Size_off.front());

	//
	
	return 1;
}

bool sdk::util::c_address_gathering::gather_pointer_scans()
{
	/*auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!main_actor) return 0;
	auto python_map = sdk::game::c_utils::Instance().baseclass_python_map();
	if (!python_map) return 0;
	if (!sdk::game::minimap_offsets::off_ATLAS_INSTANCE)
	{
		for (auto a = 0; a < 0x30; a += 0x4)
		{
			auto offset = sdk::game::minimap_offsets::off_SHOW_ATLAS + a + 0x8;
			uint32_t addr = 0;
			memcpy((void*)addr, (void*)(python_map + offset), 4);
			if (!addr || addr < (uintptr_t)GetModuleHandleA(0)) continue;
			if (addr == 0xc000000) continue;
			sdk::util::
		}
		return 0;
	}*/

	return 1;
}

bool sdk::util::c_address_gathering::check_baseclasses()
{
	return 1;
}
