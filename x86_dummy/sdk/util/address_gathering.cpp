#include "address_gathering.h"
void sdk::util::c_address_gathering::setup()
{
	auto r = this->gather_connection_related();
	if (!r) { sdk::util::c_log::Instance().duo(XorStr("[ failed gather_connection_related ]\n")); return; }
	sdk::util::c_log::Instance().duo(XorStr("[ c_address_gathering::setup completed ]\n"));
	done = true;
}

bool sdk::util::c_address_gathering::error_out(int line)
{
	sdk::util::c_log::Instance().duo(XorStr("[ failed on: %i ]\n"), line);
	return 0;
}

uint32_t sdk::util::c_address_gathering::find_singleton_or_instance(uint32_t f)
{
	auto off = (uint32_t)0x0;

	auto singleton_fn = sdk::util::c_fn_discover::Instance().discover_fn(f, 0x8, 0x9, 0, 1, 1, 0, 1);
	if (singleton_fn)
	{
		auto off_inside = sdk::util::c_disassembler::Instance().get_custom(singleton_fn, 0, 0, 0, { XorStr("mov") });
		if (off_inside.empty()) return 0;
		off = off_inside.front();
	}
	else
	{
		auto off_inside = sdk::util::c_disassembler::Instance().get_custom(f, 0, 0, 0, { XorStr("mov") });
		if (off_inside.empty()) return 0;
		off = off_inside.front();
	}

	return off;
}

bool sdk::util::c_address_gathering::gather_connection_related()
{
	auto connectaccountserver_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("ConnectToAccountServer"));
	if (!connectaccountserver_fn) return this->error_out(__LINE__);
	auto CAccountConnector_connect = sdk::util::c_fn_discover::Instance().discover_fn(connectaccountserver_fn, 0x50, 0x70, 3);
	if (!CAccountConnector_connect)
	{
		auto CAccountConnector_connect_calls = sdk::util::c_disassembler::Instance().get_calls(connectaccountserver_fn, 0, 0, 0);
		CAccountConnector_connect = CAccountConnector_connect_calls[CAccountConnector_connect_calls.size()-2];
		if (!CAccountConnector_connect) return this->error_out(__LINE__);
	}

	auto CAccountConnector_instance = this->find_singleton_or_instance(connectaccountserver_fn);
	if (!CAccountConnector_instance) return this->error_out(__LINE__);

	auto offsets = sdk::util::c_disassembler::Instance().get_custom(CAccountConnector_connect, 0, 0xA0, 0xFF, { "mov", "add", "push", "lea" });
	if (!((int)offsets.size() >= 2)) return this->error_out(__LINE__);

	auto CNetworkStream__Connect = sdk::util::c_fn_discover::Instance().get_fn(XorStr("error != WSAEWOULDBLOCK"));
	if (!CNetworkStream__Connect) return this->error_out(__LINE__);
	auto CNetworkStream__Connect_off = sdk::util::c_disassembler::Instance().get_custom(CNetworkStream__Connect, 0, 0x40, 0xA0, { "mov", "add", "push", "lea" });
	if (!CNetworkStream__Connect_off.size()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_IP = offsets[0];
	sdk::game::connection_offsets::off_PORT = offsets[1];
	sdk::game::connection_offsets::off_SOCKET = CNetworkStream__Connect_off.front();
	sdk::game::pointer_offsets::off_CAccountConnector = CAccountConnector_instance;
	sdk::game::func::c_funcs::Instance().f_ConnectToAccountServer = decltype(sdk::game::func::c_funcs::Instance().f_ConnectToAccountServer)(CAccountConnector_connect);

	sdk::util::c_log::Instance().duo(XorStr("[ off_IP        : %04x ]\n"), sdk::game::connection_offsets::off_IP);
	sdk::util::c_log::Instance().duo(XorStr("[ off_PORT      : %04x ]\n"), sdk::game::connection_offsets::off_PORT);
	sdk::util::c_log::Instance().duo(XorStr("[ off_SOCKET    : %04x ]\n"), sdk::game::connection_offsets::off_SOCKET);
	sdk::util::c_log::Instance().duo(XorStr("[ connector fn  : %04x ]\n"), CAccountConnector_connect);
	sdk::util::c_log::Instance().duo(XorStr("[ connector inst: %04x ]\n"), CAccountConnector_instance);

	//

	auto directenter_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("DirectEnter"));
	if (!directenter_fn)  return this->error_out(__LINE__);
	auto dodirectenter_fn = sdk::util::c_fn_discover::Instance().discover_fn(directenter_fn, 0x30, 0x65, 2, 0);
	if (!dodirectenter_fn)
	{
		dodirectenter_fn = sdk::util::c_fn_discover::Instance().discover_fn(directenter_fn, 0x30, 0x65, 1);
		if (!dodirectenter_fn) return this->error_out(__LINE__);
	}
	auto ConnectToGameServer_off = sdk::util::c_disassembler::Instance().get_custom(dodirectenter_fn, 0, 0xFFF, 0xFFFF, { "mov", "add", "push", "lea" });
	if (!ConnectToGameServer_off.size()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_SELECT_CHAR_IDX = ConnectToGameServer_off.front();
	sdk::game::func::c_funcs::Instance().f_ConnectToGameServer = decltype(sdk::game::func::c_funcs::Instance().f_ConnectToGameServer)(dodirectenter_fn);

	sdk::util::c_log::Instance().duo(XorStr("[ dodirectenter fn: %04x ]\n"), dodirectenter_fn);
	sdk::util::c_log::Instance().duo(XorStr("[ off_SELECT_CHAR_IDX fn: %04x ]\n"), sdk::game::connection_offsets::off_SELECT_CHAR_IDX);

	//

	auto CAccountConnector_SetLoginInfo = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetLoginInfo"));
	if (!CAccountConnector_SetLoginInfo) return this->error_out(__LINE__);

	sdk::util::c_log::Instance().duo(XorStr("[ SetLoginInfo: %04x ]\n"), CAccountConnector_SetLoginInfo);

	auto CAccountConnector_Set = sdk::util::c_disassembler::Instance().get_calls(CAccountConnector_SetLoginInfo, 0, 0);
	if (!CAccountConnector_Set.size()) return this->error_out(__LINE__);

	auto CAccountConnector_Set_off = sdk::util::c_disassembler::Instance().get_custom(CAccountConnector_Set[CAccountConnector_Set.size() - 2], 0, 0x50, 0xFF, { "mov", "add", "push", "lea" });
	if (CAccountConnector_Set_off.empty()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_USERNAME = CAccountConnector_Set_off.front();
	sdk::game::connection_offsets::off_PASSWORD = CAccountConnector_Set_off[1];
	if (CAccountConnector_Set_off.size() > 2) sdk::game::connection_offsets::off_PASSCODE = CAccountConnector_Set_off[2];

	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_accnet = decltype(sdk::game::func::c_funcs::Instance().f_SetLoginInfo_accnet)(CAccountConnector_Set[CAccountConnector_Set.size() - 2]);
	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_pynet = decltype(sdk::game::func::c_funcs::Instance().f_SetLoginInfo_pynet)(CAccountConnector_Set[CAccountConnector_Set.size() - 3]);
	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_accnet = decltype(sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_accnet)(CAccountConnector_Set[CAccountConnector_Set.size() - 2]);
	sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_pynet = decltype(sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_accnet)(CAccountConnector_Set[CAccountConnector_Set.size() - 3]);

	sdk::util::c_log::Instance().duo(XorStr("[ f_SetLoginInfo_accnet: %04x ]\n"), CAccountConnector_Set[CAccountConnector_Set.size() - 2]);
	sdk::util::c_log::Instance().duo(XorStr("[ f_SetLoginInfo_pynet : %04x ]\n"), CAccountConnector_Set[CAccountConnector_Set.size() - 3]);

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("xaleas")))//custom swapped params
	{
		sdk::game::connection_offsets::off_USERNAME = CAccountConnector_Set_off[1];
		sdk::game::connection_offsets::off_PASSWORD = CAccountConnector_Set_off[2];
		sdk::game::connection_offsets::off_PASSCODE = CAccountConnector_Set_off[0];

		sdk::util::c_log::Instance().duo(XorStr("[ xaleas off_USERNAME: %04x ]\n"), sdk::game::connection_offsets::off_PASSWORD);
		sdk::util::c_log::Instance().duo(XorStr("[ xaleas off_PASSWORD: %04x ]\n"), sdk::game::connection_offsets::off_PASSCODE);
		sdk::util::c_log::Instance().duo(XorStr("[ xaleas off_PASSCODE: %04x ]\n"), sdk::game::connection_offsets::off_USERNAME);
	}
	else
	{
		sdk::util::c_log::Instance().duo(XorStr("[ off_USERNAME: %04x ]\n"), sdk::game::connection_offsets::off_USERNAME);
		sdk::util::c_log::Instance().duo(XorStr("[ off_PASSWORD: %04x ]\n"), sdk::game::connection_offsets::off_PASSWORD);
		if (CAccountConnector_Set_off.size() > 2) sdk::util::c_log::Instance().duo(XorStr("[ off_PASSCODE: %04x ]\n"), sdk::game::connection_offsets::off_PASSCODE);
	}
	//

	auto CPythonNetworkStream_SetMarkServer = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetMarkServer"));
	if (!CPythonNetworkStream_SetMarkServer) return this->error_out(__LINE__);
	auto CPythonNetworkStream_SetMarkServer_calls = sdk::util::c_disassembler::Instance().get_calls(CPythonNetworkStream_SetMarkServer, 0, 0);
	if (CPythonNetworkStream_SetMarkServer_calls.empty())
	{
		CPythonNetworkStream_SetMarkServer_calls = sdk::util::c_disassembler::Instance().get_jumps(CPythonNetworkStream_SetMarkServer, 0, 0);
		if (CPythonNetworkStream_SetMarkServer_calls.empty()) return this->error_out(__LINE__);
	}

	auto CPythonNetworkStream_instance = this->find_singleton_or_instance(CPythonNetworkStream_SetMarkServer);
	if (!CPythonNetworkStream_instance) return this->error_out(__LINE__);

	sdk::game::pointer_offsets::off_CPythonNetworkStream = CPythonNetworkStream_instance;
	sdk::game::func::c_funcs::Instance().f_SetMarkServer = decltype(sdk::game::func::c_funcs::Instance().f_SetMarkServer)(CPythonNetworkStream_SetMarkServer_calls[CPythonNetworkStream_SetMarkServer_calls.size() - 2]);
	sdk::util::c_log::Instance().duo(XorStr("[ f_SetMarkServer: %04x ]\n"), CPythonNetworkStream_SetMarkServer_calls[CPythonNetworkStream_SetMarkServer_calls.size() - 2]);
	sdk::util::c_log::Instance().duo(XorStr("[ off_CPythonNetworkStream: %04x ]\n"), CPythonNetworkStream_instance);

	//

	auto CPythonNetworkStream_SetOffLinePhase = sdk::util::c_fn_discover::Instance().get_fn(XorStr("## Network - OffLine Phase ##"));
	if (!CPythonNetworkStream_SetOffLinePhase)
	{
		//todo find good alt ref
		auto SetOfflinePhase_py = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetOfflinePhase"));
		if (!SetOfflinePhase_py) return this->error_out(__LINE__);

		auto SetOfflinePhase_py_calls = sdk::util::c_disassembler::Instance().get_calls(SetOfflinePhase_py, 0xB, 0, 0);
		if (SetOfflinePhase_py_calls.empty()) return this->error_out(__LINE__);

		for (auto b : SetOfflinePhase_py_calls) sdk::util::c_log::Instance().duo("[ %04x ]\n", b);

		if (SetOfflinePhase_py_calls.size() > 1) CPythonNetworkStream_SetOffLinePhase = SetOfflinePhase_py_calls[SetOfflinePhase_py_calls.size()-2];
		else CPythonNetworkStream_SetOffLinePhase = SetOfflinePhase_py_calls.front();
		if (!CPythonNetworkStream_SetOffLinePhase) return this->error_out(__LINE__);
	}

	auto CPythonNetworkStream_SetOffLinePhase_off = sdk::util::c_disassembler::Instance().get_custom(CPythonNetworkStream_SetOffLinePhase, 0, 0x50, 0x1FF, { "mov", "add", "push", "lea" });
	if (CPythonNetworkStream_SetOffLinePhase_off.empty()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_NETWORKING_PHASE = CPythonNetworkStream_SetOffLinePhase_off.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_NETWORKING_PHASE: %04x ]\n"), CPythonNetworkStream_SetOffLinePhase_off.front());
	sdk::util::c_log::Instance().duo(XorStr("[ CPythonNetworkStream_SetOffLinePhase: %04x ]\n"), CPythonNetworkStream_SetOffLinePhase);

	//

	auto CNetworkStream_IsOnline = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("IsConnect"));
	if (!CNetworkStream_IsOnline) return this->error_out(__LINE__);

	auto CNetworkStream_IsOnline_ = sdk::util::c_fn_discover::Instance().discover_fn(CNetworkStream_IsOnline, 0x2, 0x4);
	if (!CNetworkStream_IsOnline_) return this->error_out(__LINE__);

	auto CNetworkStream_IsOnline_off = sdk::util::c_disassembler::Instance().get_custom(CNetworkStream_IsOnline_, 0, 0x20, 0xFF, { "mov", "add", "push", "lea" });
	if (CNetworkStream_IsOnline_off.empty()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_IS_CONNECTED = CNetworkStream_IsOnline_off.front();
	sdk::util::c_log::Instance().duo(XorStr("[ off_IS_CONNECTED: %04x ]\n"), CNetworkStream_IsOnline_off.front());

	//

	auto CPythonNetworkStream_SendSelectCharacterPacket = sdk::util::c_fn_discover::Instance().get_fn(XorStr("SendSelectCharacterPacket - Error"));
	if (!CPythonNetworkStream_SendSelectCharacterPacket) return this->error_out(__LINE__);

	sdk::game::func::c_funcs::Instance().f_SendSelectCharacter = decltype(sdk::game::func::c_funcs::Instance().f_SendSelectCharacter)(CPythonNetworkStream_SendSelectCharacterPacket);
	sdk::game::func::c_funcs::Instance().o_SendSelectCharacter = CPythonNetworkStream_SendSelectCharacterPacket;
	
	sdk::util::c_log::Instance().duo(XorStr("[ CPythonNetworkStream_SendSelectCharacterPacket: %04x ]\n"), CPythonNetworkStream_SendSelectCharacterPacket);

	//

	auto SetPacketSequenceMode_fn = sdk::util::c_fn_discover::Instance().get_fn_py(XorStr("SetPacketSequenceMode"));
	if (SetPacketSequenceMode_fn)
	{
		auto SetPacketSequenceMode_calls = sdk::util::c_fn_discover::Instance().discover_fn(SetPacketSequenceMode_fn, 0xA, 0xF, 0);
		if (SetPacketSequenceMode_calls)
		{
			auto SetPacketSequenceMode_off = sdk::util::c_disassembler::Instance().get_custom(SetPacketSequenceMode_calls, 0, 0x30, 0xFF, { "mov", "add", "push", "lea" });
			if (!SetPacketSequenceMode_off.empty())
			{
				sdk::game::connection_offsets::off_PACKET_SEQUENCE_MODE = SetPacketSequenceMode_off.front();
				sdk::util::c_log::Instance().duo(XorStr("[ off_PACKET_SEQUENCE_MODE: %04x ]\n"), SetPacketSequenceMode_off.front());
			}
		}
	}

	return 1;
}

bool sdk::util::c_address_gathering::gather_actor_related()
{


	return 1;
}
