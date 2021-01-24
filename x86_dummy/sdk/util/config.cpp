#include "config.h"

void sdk::util::c_config::setup()
{
	this->registrant();
	this->load();
	this->save();
	sdk::util::c_log::Instance().duo(XorStr("[ c_config::setup completed ]\n"));
}

void sdk::util::c_config::add_variable(std::string h, std::string v, std::string c, uint8_t t)
{
	this->conf_variables.push_back(new sdk::util::json_cfg::s_config_value(h, v, c, t));
}

void sdk::util::c_config::registrant()
{
	this->add_variable(XorStr("dynamics"), XorStr("last_file_crc"), "0", CONF_TYPE_STR);
}

bool sdk::util::c_config::save()
{
	this->fstream.open(this->file_name);
	if (!this->fstream.is_open()) { util::c_log::Instance().duo(XorStr("[ failed to open conf stream ]\n")); return 0; }
	for (auto a : this->conf_variables)
	{
		if (a->variable.empty() || a->header.empty()) continue;
		auto jobj = nlohmann::json();
		jobj["header"] = a->header;
		jobj["variable"] = a->variable;
		jobj["container"] = a->container;
		jobj["type"] = a->type;
		this->fstream << jobj << "\n";
	}
	this->fstream.close();
	util::c_log::Instance().print(XorStr("[ saved config ]\n"));
	return 1;
}

bool sdk::util::c_config::load()
{
	this->ifstream.open(this->file_name);
	if (!this->ifstream.is_open()) { util::c_log::Instance().duo(XorStr("[ failed to open conf stream ]\n")); return 0; }
	std::string s;
	while (std::getline(this->ifstream, s))
	{
		if (s.empty() || s.size() < 3) continue;
		auto struct_obj = util::json_cfg::s_config_value();
		try {
			nlohmann::json::parse(s).get_to(struct_obj);
		}
		catch (...) { continue; }
		if (struct_obj.header.empty() || struct_obj.variable.empty()) continue;
		for (auto&& a : this->conf_variables)
		{/*update our container*/
			if (a->header == struct_obj.header && a->variable == struct_obj.variable)
			{
				a->container = struct_obj.container;
				break;
			}
		}
	}
	this->ifstream.close();
	util::c_log::Instance().print(XorStr("[ loaded config ]\n"));
	return 1;
}

sdk::util::t_conf_var sdk::util::c_config::get_var(const char* h, const char* v)
{
	for (auto&& a : this->conf_variables) if (a->header == h && a->variable == v) return a;
	sdk::util::c_log::Instance().duo(XorStr("[ failed to find %s, %s ]\n"), h, v);
	return nullptr;
}
