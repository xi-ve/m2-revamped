#include "address_gathering.h"
void sdk::util::c_address_gathering::setup()
{
	auto r = this->gather_connection_related();
	if (!r) { sdk::util::c_log::Instance().duo("[ failed gather_connection_related ]\n"); return; }
	sdk::util::c_log::Instance().duo("[ c_address_gathering::setup completed ]\n");
}

bool sdk::util::c_address_gathering::error_out(int line)
{
	sdk::util::c_log::Instance().duo("[ failed on: %i ]\n", line);
	return 1;
}

uint32_t sdk::util::c_address_gathering::find_singleton_or_instance(uint32_t f)
{
	auto off = (uint32_t)0x0;

	auto singleton_fn = sdk::util::c_fn_discover::Instance().discover_fn(f, 0x8, 0x9, 0, 1, 1, 0, 1);
	if (singleton_fn)
	{
		auto off_inside = sdk::util::c_disassembler::Instance().get_custom(singleton_fn, 0, 0, 0, { "mov" });
		if (off_inside.empty()) return 0;
		off = off_inside.front();
	}
	else
	{
		auto off_inside = sdk::util::c_disassembler::Instance().get_custom(f, 0, 0, 0, { "mov" });
		if (off_inside.empty()) return 0;
		off = off_inside.front();
	}

	return off;
}

bool sdk::util::c_address_gathering::gather_connection_related()
{
	auto connectaccountserver_fn = sdk::util::c_fn_discover::Instance().get_fn_py("ConnectToAccountServer");
	if (!connectaccountserver_fn) return this->error_out(__LINE__);
	auto CAccountConnector_connect = sdk::util::c_fn_discover::Instance().discover_fn(connectaccountserver_fn, 0x50, 0x70, 4);
	if (!CAccountConnector_connect) return this->error_out(__LINE__);

	auto CAccountConnector_instance = this->find_singleton_or_instance(connectaccountserver_fn);
	if (!CAccountConnector_instance) return this->error_out(__LINE__);

	auto offsets = sdk::util::c_disassembler::Instance().get_custom(CAccountConnector_connect, 0, 0xA0, 0xFF, { "mov", "add", "push", "lea" });
	if (!((int)offsets.size() >= 2)) return this->error_out(__LINE__);

	auto CNetworkStream_Connect = sdk::util::c_fn_discover::Instance().discover_fn(CAccountConnector_connect, 0x70, 0x80, 4, 0);
	if (!CNetworkStream_Connect) return this->error_out(__LINE__);

	sdk::util::c_log::Instance().duo("[ CAccountConnector_connect %04x ]\n", CAccountConnector_connect);
	sdk::util::c_log::Instance().duo("[ CNetworkStream_Connect %04x ]\n", CNetworkStream_Connect);

	auto CNetworkStream_Connect_ = sdk::util::c_fn_discover::Instance().discover_fn(CNetworkStream_Connect, 0x90, 0xB0, 3);
	if (!CNetworkStream_Connect_) return this->error_out(__LINE__);

	auto CNetworkStream_Connect_offsets = sdk::util::c_disassembler::Instance().get_custom(CNetworkStream_Connect_, 0, 0x30, 0xA0, { "lea" });
	if (CNetworkStream_Connect_offsets.empty()) return this->error_out(__LINE__);

	sdk::game::connection_offsets::off_IP = offsets[0];
	sdk::game::connection_offsets::off_PORT = offsets[1];
	sdk::game::connection_offsets::off_SOCKET = CNetworkStream_Connect_offsets.front();
	sdk::game::pointer_offsets::off_CAccountConnector = CAccountConnector_instance;
	sdk::game::func::c_funcs::Instance().f_ConnectToAccountServer = decltype(sdk::game::func::c_funcs::Instance().f_ConnectToAccountServer)(CAccountConnector_connect);

	sdk::util::c_log::Instance().duo("[ off_IP        : %04x ]\n", sdk::game::connection_offsets::off_IP);
	sdk::util::c_log::Instance().duo("[ off_PORT      : %04x ]\n", sdk::game::connection_offsets::off_PORT);
	sdk::util::c_log::Instance().duo("[ off_SOCKET    : %04x ]\n", sdk::game::connection_offsets::off_SOCKET);
	sdk::util::c_log::Instance().duo("[ connector fn  : %04x ]\n", CAccountConnector_connect);
	sdk::util::c_log::Instance().duo("[ connector inst: %04x ]\n", CAccountConnector_instance);

	//

	auto directenter_fn = sdk::util::c_fn_discover::Instance().get_fn_py("DirectEnter");
	if (!directenter_fn)  return this->error_out(__LINE__);
	auto dodirectenter_fn = sdk::util::c_fn_discover::Instance().discover_fn(directenter_fn, 0x30, 0x65, 2, 0);
	if (!dodirectenter_fn) return this->error_out(__LINE__);

	sdk::game::func::c_funcs::Instance().f_Connect = decltype(sdk::game::func::c_funcs::Instance().f_Connect)(dodirectenter_fn);

	sdk::util::c_log::Instance().duo("[ dodirectenter fn: %04x ]\n", dodirectenter_fn);

	return 1;
}
