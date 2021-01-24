#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		namespace json_cfg
		{
			class s_config_value
			{
			public:
				s_config_value(){}
				s_config_value(std::string h, std::string v, std::string c, uint8_t t)
				{
					header = h; variable = v; container = c; type = t;
				}
				std::string		header = "";
				std::string		variable = "";
				std::string		container = "";
				uint8_t			type = 0;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(s_config_value, header, variable, container, type);
			};
		}
		enum 
		{
			CONF_TYPE_INT = 0,
			CONF_TYPE_STR = 1,
			CONF_TYPE_FLT = 2
		};
		typedef std::vector<json_cfg::s_config_value*>	t_conf;
		typedef json_cfg::s_config_value*				t_conf_var;
		class c_config : public s<c_config>
		{
		private:
			std::string		file_name = XorStr("M2++_CONF.TXT");
			t_conf			conf_variables;
			std::ofstream	fstream;
			std::ifstream	ifstream;
		private:
			void			add_variable(std::string h, std::string v, std::string c, uint8_t t);
			void			registrant();
		public:
			void			setup();
			bool			save();
			bool			load();
			t_conf_var		get_var(const char* h, const char* v);
		};
	}
}