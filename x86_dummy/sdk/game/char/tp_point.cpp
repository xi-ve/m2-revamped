#include "tp_point.h"

void sdk::game::chr::c_tp_point::setup()
{
	this->points.clear();
	this->last_point = sdk::util::c_config::Instance().get_var(XorStr("tp_point"), XorStr("last_used"));
	this->load();
}

void sdk::game::chr::c_tp_point::work(std::string point)
{
	auto p = this->get_point(point);
	if (!p.valid()) return;
	auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
	if (!main_actor) return;
	auto background_base = sdk::game::c_utils::Instance().baseclass_background();
	if (background_base)
	{
		auto bg_name = *(std::string*)(background_base + sdk::game::background::off_CUR_MAP_NAME);

		sdk::util::c_log::Instance().duo("[ %s - %s ]\n", bg_name.c_str(), this->get_point_map(point).c_str());

		if (!strstr(bg_name.c_str(), this->get_point_map(point).c_str())) return;

		sdk::game::c_waithack::Instance().interpolate_to_pos(sdk::game::chr::c_char::Instance().get_pos(main_actor), p);

		auto a = (sdk::util::json_cfg::s_config_value*)(this->last_point);
		a->container = point;
	}
}

sdk::game::vec sdk::game::chr::c_tp_point::get_point(std::string name)
{
	for (auto a : this->points) if (a.point_name == name) return sdk::util::math::c_vector3(std::stof(a.x), std::stof(a.y), std::stof(a.z));
	return {};
}

std::string sdk::game::chr::c_tp_point::get_point_map(std::string name)
{
	for (auto a : this->points) if (a.point_name == name) return a.map_name;
	return {};
}

std::string sdk::game::chr::c_tp_point::get_last_point()
{
	auto r = (sdk::util::json_cfg::s_config_value*)(this->last_point);
	return r->container;
}

void sdk::game::chr::c_tp_point::add(float x, float y, float z, std::string name)
{
	auto background_base = sdk::game::c_utils::Instance().baseclass_background();
	if (background_base)
	{
		auto bg_name = *(std::string*)(background_base + sdk::game::background::off_CUR_MAP_NAME);
		sdk::game::chr::json_tp_points::s_tp_point_cfg t((int)x, (int)y, (int)z, name, bg_name);
		this->points.push_back(t);
	}
}

sdk::game::chr::str_vec sdk::game::chr::c_tp_point::get_names()
{
	sdk::game::chr::str_vec list;
	for (auto a : this->points) list.push_back(a.point_name);
	return list;
}

void sdk::game::chr::c_tp_point::load()
{
	this->points.clear();
	std::ifstream f(this->file_name);
	std::string l = "";
	while (std::getline(f, l))
	{
		if (l.empty()) continue;
		if (l.size() < 6) continue;
		auto jparse = nlohmann::json::parse(l);
		auto asobj = jparse.get<json_tp_points::s_tp_point_cfg>();
		asobj.pos = sdk::util::math::c_vector3(std::stof(asobj.x), std::stof(asobj.y), std::stof(asobj.z));
		sdk::util::c_log::Instance().duo("[ tp loaded %s ]\n", asobj.point_name.c_str());
		this->points.push_back(asobj);
	}
	f.close();
}

void sdk::game::chr::c_tp_point::save()
{
	std::ofstream f(this->file_name);
	for (auto a : this->points)
	{
		auto j = nlohmann::json();
		nlohmann::to_json(j, a);
		f << j << "\n";		
	}
	f.close();
}
