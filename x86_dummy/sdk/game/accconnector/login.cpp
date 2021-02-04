#include "login.h"
using namespace std::chrono;

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
	if (this->accs.size())
	{
		auto last_selected = sdk::util::c_config::Instance().get_var("login", "last_character");
		if (last_selected->container.size())
		{
			for (auto a : this->accs) if (strstr(a.username.c_str(), last_selected->container.c_str())) { this->account_selected = a; break; }
		}
	}
	f.close();
}

void sdk::game::accconnector::c_login::add_acc(json::s_account a)
{
	for (auto a : this->accs) if (strstr(a.username.c_str(), a.username.c_str())) return;
	this->accs.push_back(a);
}

void sdk::game::accconnector::c_login::set_only_details()
{
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
	auto account_connector_base = sdk::game::c_utils::Instance().baseclass_account_connector();
	if (network_base && account_connector_base)
	{
		if (!this->acccon_passcode) sdk::game::func::c_funcs::Instance().f_SetLoginInfo_accnet(account_connector_base, this->account_selected.username.c_str(), this->account_selected.password.c_str());
		else sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_accnet(account_connector_base, this->account_selected.username.c_str(), this->account_selected.password.c_str(), this->account_selected.passcode.c_str());
		if (!this->pynet_passcode) sdk::game::func::c_funcs::Instance().f_SetLoginInfo_pynet(network_base, this->account_selected.username.c_str(), this->account_selected.password.c_str());
		else sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_pynet(network_base, this->account_selected.username.c_str(), this->account_selected.password.c_str(), this->account_selected.passcode.c_str());
		sdk::game::func::c_funcs::Instance().f_SetMarkServer(network_base, this->account_selected.ip_ch.c_str(), std::stoi(this->account_selected.port_ch.c_str()));
	}
}

void sdk::game::accconnector::c_login::set_details()
{
	auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
	auto account_connector_base = sdk::game::c_utils::Instance().baseclass_account_connector();
	if (network_base && account_connector_base)
	{
		if (!this->acccon_passcode) sdk::game::func::c_funcs::Instance().f_SetLoginInfo_accnet(account_connector_base, this->account_selected.username.c_str(), this->account_selected.password.c_str());
		else sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_accnet(account_connector_base, this->account_selected.username.c_str(), this->account_selected.password.c_str(), this->account_selected.passcode.c_str());
		if (!this->pynet_passcode) sdk::game::func::c_funcs::Instance().f_SetLoginInfo_pynet(network_base, this->account_selected.username.c_str(), this->account_selected.password.c_str());
		else sdk::game::func::c_funcs::Instance().f_SetLoginInfo_passcode_pynet(network_base, this->account_selected.username.c_str(), this->account_selected.password.c_str(), this->account_selected.passcode.c_str());
		sdk::game::func::c_funcs::Instance().f_SetMarkServer(network_base, this->account_selected.ip_ch.c_str(), std::stoi(this->account_selected.port_ch.c_str()));
		sdk::game::func::c_funcs::Instance().f_ConnectToAccountServer(account_connector_base, this->account_selected.ip_auth.c_str(), std::stoi(this->account_selected.port_auth.c_str()), this->account_selected.ip_ch.c_str(), std::stoi(this->account_selected.port_ch.c_str()));
		if (strstr(this->account_selected.seq_mode.c_str(), "1"))
		{
			*(uint8_t*)(network_base + sdk::game::connection_offsets::off_PACKET_SEQUENCE_MODE) = 1;
			*(uint8_t*)(account_connector_base + sdk::game::connection_offsets::off_PACKET_SEQUENCE_MODE) = 1;
		}
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
	if (network_base)
	{
		sdk::game::func::c_funcs::Instance().f_ConnectToGameServer(network_base, std::stoi(this->account_selected.slot.c_str()));
		this->f_char(network_base, std::stoi(this->account_selected.slot.c_str()));
	}
}

void sdk::game::accconnector::c_login::set_grab_details()
{
}

bool __stdcall selectchar(uint32_t ecx, BYTE id)
{
	auto py_net = sdk::game::c_utils::Instance().baseclass_networking();
	//sdk::util::c_log::Instance().duo("[ select char: %04x %04x ]\n", py_net, id);
	if (!sdk::game::accconnector::c_login::Instance().force_slot) return sdk::game::accconnector::c_login::Instance().f_char(py_net, id);
	else return sdk::game::accconnector::c_login::Instance().f_char(py_net, std::stoi(sdk::game::accconnector::c_login::Instance().account_selected.slot.c_str()));
}

__declspec(naked) bool __stdcall naked_select_hook(BYTE base, uint32_t id)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push ebx

		push dword ptr[base]
		push dword ptr[id]

		call selectchar

		pop ebx
		pop ebp

		retn 1
	}
}

void sdk::game::accconnector::c_login::work()
{
	if (this->should_grab_details)
	{
		this->read_details();
		return;
	}
	auto enabled = sdk::util::c_config::Instance().get_var("login", "enable");
	if (strstr(enabled->container.c_str(), "1") && this->account_selected.username.size() > 2)
	{
		auto py_net = sdk::game::c_utils::Instance().baseclass_networking();
		auto con_net = sdk::game::c_utils::Instance().baseclass_account_connector();
		if (!py_net || !con_net) return;

		char cur_phase[16] = "\0";
		memcpy(cur_phase, (void*)(py_net + sdk::game::connection_offsets::off_NETWORKING_PHASE), 16);

		auto online = *(uint8_t*)(py_net + sdk::game::connection_offsets::off_IS_CONNECTED);

		if (GetTickCount64() > this->last_login)
		{
			if (strstr(cur_phase, "Game"))
			{
				this->last_phase = cur_phase;
				this->last_login = GetTickCount64() + 1000;
				return;
			}
			if (strstr(this->last_phase.c_str(), "Game") && strstr(cur_phase, "Select"))
			{ //char change
				//sdk::util::c_log::Instance().duo("[ tp/char swap %s ]\n", cur_phase);
				this->last_login = GetTickCount64() + 2500;
				return;
			}
			if (strstr(this->last_phase.c_str(), "Game") && strstr(cur_phase, "HandShake"))
			{ //channel swap
				//sdk::util::c_log::Instance().duo("[ map/ch swap %s ]\n", cur_phase);
				this->last_login = GetTickCount64() + 2500;
				return;
			}

			if (!strstr(cur_phase, "OffLine") && !strstr(cur_phase, "Select")) return;

			if (!this->did_login && !online)
			{
				if (!this->enabled_hook)
				{
					MH_CreateHook((void*)sdk::game::func::c_funcs::Instance().o_SendSelectCharacter, (void*)naked_select_hook, (void**)&this->f_char);
					MH_EnableHook((void*)sdk::game::func::c_funcs::Instance().o_SendSelectCharacter);
					this->enabled_hook = true;
				}

				sdk::util::c_log::Instance().duo("[ connecting ... ]\n");
				this->set_details();
				this->last_phase = cur_phase;
				this->did_login = 1;
				return;
			}

			while (!strstr(cur_phase, "Select"))
			{
				this->last_login = GetTickCount64() + 10000;
				memcpy(cur_phase, (void*)(py_net + sdk::game::connection_offsets::off_NETWORKING_PHASE), 16);
			}

			std::this_thread::sleep_for(2s);

			this->set_character();
			this->force_slot = true;

			while (!strstr(cur_phase, "Load"))
			{
				this->last_login = GetTickCount64() + 10000;
				memcpy(cur_phase, (void*)(py_net + sdk::game::connection_offsets::off_NETWORKING_PHASE), 16);
			}

			this->did_login = false;
			this->last_login = GetTickCount64() + 10000;
			sdk::util::c_log::Instance().duo("[ login completed ]\n");

			this->last_phase = cur_phase;

		}
	}
}

void sdk::game::accconnector::c_login::setup()
{	
	this->load_accs();

	if (sdk::game::connection_offsets::off_PASSCODE) this->acccon_passcode = true;
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Kevra"))
	{
		this->acccon_passcode = true;
		this->pynet_passcode = true;
	}
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Arithra2"))
	{
		this->acccon_passcode = true;
	}
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "xaleas"))
	{
		this->acccon_passcode = true;
	}
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Celestial World 2.0"))
	{
		this->acccon_passcode = true;
		this->pynet_passcode = true;
	}
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
		if (!strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "SunshineMt2")) memcpy(cur_password, (void*)(account_connector_base + sdk::game::connection_offsets::off_PASSWORD), 32);
		else
		{
			auto r = *(std::string*)(account_connector_base + sdk::game::connection_offsets::off_PASSWORD);
			strcpy(cur_password, r.c_str());
		}

		if (!cur_password || cur_password[0] == 0 || !strlen(cur_password)) return;

		char cur_passcode[32] = "\0";
		if (sdk::game::connection_offsets::off_PASSCODE && !strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Celestial World 2.0"))
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

		auto seq_mode = 0;
		if (sdk::game::connection_offsets::off_PACKET_SEQUENCE_MODE) seq_mode = *(uint8_t*)(network_base + sdk::game::connection_offsets::off_PACKET_SEQUENCE_MODE);

		this->account_last_grabbed = json::s_account((std::string)cur_username,
													 (std::string)cur_password,
													 (std::string)cur_passcode,
													 "0",
													 (std::string)cur_ip,
													 (std::string)net_ip,
													 (std::string)std::to_string(cur_port),
													 (std::string)std::to_string(port),
													 (std::string)std::to_string(seq_mode));

		sdk::util::c_log::Instance().duo("[ logged info, user %s pass %s ch ip %s auth ip %s ch port %s auth port %s seq %s ]\n", this->account_last_grabbed.username.c_str(), this->account_last_grabbed.password.c_str(), this->account_last_grabbed.ip_ch.c_str(), this->account_last_grabbed.ip_auth.c_str(), this->account_last_grabbed.port_ch.c_str(), this->account_last_grabbed.port_auth.c_str(), this->account_last_grabbed.seq_mode.c_str());
		this->add_acc(this->account_last_grabbed);
		this->save_accs();
		auto v = sdk::util::c_config::Instance().get_var("login", "last_character");
		v->container = cur_username;
		sdk::util::c_config::Instance().save();

		this->should_grab_details = false;
	}
}
