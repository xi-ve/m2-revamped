#include "login.h"

void sdk::game::accconnector::c_login::save_accs()
{
	for (auto a : this->accs)
	{
		auto j = nlohmann::json();
		nlohmann::to_json(j, a);
		std::ofstream f(this->accs_file);
		f << j << "\n";
		f.close();
	}
}

void sdk::game::accconnector::c_login::load_accs()
{
	this->accs.clear();
	std::ifstream f(this->accs_file);
	std::string l = "";
	while (std::getline(f, l))
	{
		if (l.empty()) continue;
		if (l.size() < 14) continue;
		auto jparse = nlohmann::json::parse(l);
		auto asobj = jparse.get<json::s_account>();
		sdk::util::c_log::Instance().duo("[ loaded %s ]\n", asobj.username.c_str());
		this->accs.push_back(asobj);
	}
	if (this->accs.size()) this->account_selected = this->accs.front();
	f.close();
}

void sdk::game::accconnector::c_login::add_acc(json::s_account a)
{
	for (auto a : this->accs) if (strstr(a.username.c_str(), a.username.c_str())) return;
	this->accs.push_back(a);
}

void sdk::game::accconnector::c_login::set_details()
{
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
	auto account_connector_base = sdk::game::c_utils::Instance().baseclass_account_connector();
	if (network_base && account_connector_base)
	{
		sdk::game::func::c_funcs::Instance().f_SetLoginInfo_accnet(account_connector_base, this->account_selected.username.c_str(), this->account_selected.password.c_str());
		sdk::game::func::c_funcs::Instance().f_SetLoginInfo_pynet(network_base, this->account_selected.username.c_str(), this->account_selected.password.c_str());
		sdk::game::func::c_funcs::Instance().f_SetMarkServer(network_base, this->account_selected.ip_ch.c_str(), std::stoi(this->account_selected.port_ch.c_str()));
		sdk::game::func::c_funcs::Instance().f_ConnectToAccountServer(account_connector_base, this->account_selected.ip_auth.c_str(), std::stoi(this->account_selected.port_auth.c_str()), this->account_selected.ip_ch.c_str(), std::stoi(this->account_selected.port_ch.c_str()));
	}
}

void sdk::game::accconnector::c_login::set_connect()
{
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
	if (network_base) sdk::game::func::c_funcs::Instance().f_ConnectToGameServer(network_base, std::stoi(this->account_selected.slot.c_str()));
}

void sdk::game::accconnector::c_login::set_character()
{
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
	if (network_base) sdk::game::func::c_funcs::Instance().f_SendSelectCharacter(network_base, std::stoi(this->account_selected.slot.c_str()));
}

void sdk::game::accconnector::c_login::set_grab_details()
{
}

void sdk::game::accconnector::c_login::work()
{
	if (this->should_grab_details) this->read_details();
}

bool __stdcall selectchar(uint32_t ecx, uint32_t id)
{
	printf("[ select char call from engine id: %04x -> %04x ]\n", ecx, id);
	return sdk::game::accconnector::c_login::Instance().f_char(id, ecx);
}

__declspec(naked) bool __stdcall naked_select_hook()
{
	__asm
	{
		pushfd;
		pushad;

		popad;
		popfd;

		jmp[selectchar];
	}
}

void sdk::game::accconnector::c_login::setup()
{
	//MH_CreateHook((void*)sdk::game::func::c_funcs::Instance().o_SendSelectCharacter, (void*)naked_select_hook, (void**)&this->f_char);
	//MH_EnableHook((void*)sdk::game::func::c_funcs::Instance().o_SendSelectCharacter);
	this->load_accs();
}

void sdk::game::accconnector::c_login::read_details()
{
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
	auto account_connector_base = sdk::game::c_utils::Instance().baseclass_account_connector();
	if (network_base && account_connector_base)
	{
		char net_phase[16] = "\0";		
		memcpy(net_phase, (void*)(network_base + sdk::game::connection_offsets::off_NETWORKING_PHASE), 16);

		if (!net_phase) return;

		char cur_username[32] = "\0";
		memcpy(cur_username, (void*)(account_connector_base + sdk::game::connection_offsets::off_USERNAME), 32);
		
		if (!cur_username || cur_username[0] == 0 || !strlen(cur_username)) return;

		char cur_password[32] = "\0";
		memcpy(cur_password, (void*)(account_connector_base + sdk::game::connection_offsets::off_PASSWORD), 32);
		
		if (!cur_password || cur_password[0] == 0 || !strlen(cur_password)) return;

		char cur_passcode[32] = "\0";
		if (sdk::game::connection_offsets::off_PASSCODE)
		{
			memcpy(cur_passcode, (void*)(account_connector_base + sdk::game::connection_offsets::off_PASSCODE), 32);
			if (!cur_passcode || cur_passcode[0] == 0 || !strlen(cur_passcode)) return;
		}

		char cur_ip[32] = "\0";
		memcpy(cur_ip, (void*)(account_connector_base + sdk::game::connection_offsets::off_IP), 32);

		if (!cur_ip || !sdk::util::c_fn_discover::Instance().is_ascii(cur_ip))
		{
			auto r = *(std::string*)(account_connector_base + sdk::game::connection_offsets::off_IP);
			strcpy(cur_ip, r.c_str());
		}

		auto cur_port = *(uint32_t*)(account_connector_base + sdk::game::connection_offsets::off_PORT);

		if (!cur_port) return;

		auto sock_auth_srv = *(sockaddr_in*)(account_connector_base + sdk::game::connection_offsets::off_SOCKET);

		auto net_ip = inet_ntoa(sock_auth_srv.sin_addr);
		if (!net_ip || !strlen(net_ip)) return;

		auto port = htons(sock_auth_srv.sin_port);
		if (!port) return;

		this->account_last_grabbed = json::s_account((std::string)cur_username,
													 (std::string)cur_password,
													 (std::string)cur_passcode,
													 "0",
													 (std::string)cur_ip,
													 (std::string)net_ip,
													 (std::string)std::to_string(cur_port),
													 (std::string)std::to_string(port));

		sdk::util::c_log::Instance().duo("[ logged info, user %s pass %s ch ip %s auth ip %s ch port %s auth port %s ]\n", this->account_last_grabbed.username.c_str(), this->account_last_grabbed.password.c_str(), this->account_last_grabbed.ip_ch.c_str(), this->account_last_grabbed.ip_auth.c_str(), this->account_last_grabbed.port_ch.c_str(), this->account_last_grabbed.port_auth.c_str());
		this->add_acc(this->account_last_grabbed);
		this->save_accs();

		this->should_grab_details = false;
	}
}

void sdk::game::accconnector::c_login::read_server_details()
{
}