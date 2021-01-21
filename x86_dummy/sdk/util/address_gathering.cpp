#include "address_gathering.h"
void sdk::util::c_address_gathering::setup()
{
	auto r = this->gather_connection_related();
	if (!r) return;
	sdk::util::c_log::Instance().duo("[ c_address_gathering::setup completed ]\n");
}

bool sdk::util::c_address_gathering::gather_connection_related()
{
	auto connectaccountserver_fn = sdk::util::c_fn_discover::Instance().get_fn_py("ConnectToAccountServer");
	if (!connectaccountserver_fn) return 0;
	auto CAccountConnector_connect = sdk::util::c_fn_discover::Instance().discover_fn(connectaccountserver_fn, 0x50, 0x70, 4);
	sdk::util::c_log::Instance().duo("[ connector fn: %04x ]\n", CAccountConnector_connect);
	return 1;
}
