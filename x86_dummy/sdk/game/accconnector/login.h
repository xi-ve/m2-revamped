#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace accconnector
		{
			namespace json
			{
				struct s_account
				{
					s_account() { }
					s_account(std::string a, std::string b, std::string c, std::string d, std::string e, std::string f, std::string g, std::string h, std::string xf)
					{
						username = a;
						password = b;
						passcode = c;
						slot = d;
						ip_auth = e;
						ip_ch = f;
						port_auth = g;
						port_ch = h;
						seq_mode = xf;
					}
					std::string		username = "";
					std::string		password = "";
					std::string		passcode = "";
					std::string		slot = "";
					std::string		ip_auth = "";
					std::string		ip_ch = "";
					std::string		port_auth = "";
					std::string		port_ch = "";
					std::string		seq_mode = "";
					NLOHMANN_DEFINE_TYPE_INTRUSIVE(s_account, username, password, passcode, slot, ip_auth, ip_ch, port_auth, port_ch, seq_mode);
				};
			}
			typedef sdk::game::func::t::t_SendSelectCharacter	t_char;
			typedef std::vector< json::s_account>				t_accs;
			class c_login : public s<c_login>
			{
			private:
				bool				pynet_passcode = false;
				bool				acccon_passcode = false;
				const char*			accs_file = "M2++_ACCS.DB";
				t_accs				accs;
				json::s_account		account_last_grabbed;
			private:
				void				read_details();
			private:
				bool				did_login = false;
				bool				did_char_select = false;
				bool				should_login = false;
				bool				enabled_hook = false;
				ULONGLONG			last_login = 0;
				ULONGLONG			initial_delay = 0;
				std::string			last_phase = "";
			public:
				void				save_accs();
				void				load_accs();
				void				add_acc(json::s_account a);
			public:
				void				set_only_details();
				void				set_details();
				void				set_connect();
				void				set_character();
			public:
				t_char				f_char;
				int					should_grab_details = false;
				bool				force_slot = false;
				json::s_account		account_selected;
				void				set_account(json::s_account a) { this->account_last_grabbed = a; };
				void				set_grab_details();
				void				work();
				void				setup();
			};
		}
	}
}