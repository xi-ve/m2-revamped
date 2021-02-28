#include "map_files.h"

sdk::game::map::s_img_data* sdk::game::map::c_map_files::find(std::string name)
{
	for (auto && a : this->textures)
	{
		if (strstr(a.path.c_str(), name.c_str())) return &a;
	}
	return nullptr;
}

void sdk::game::map::c_map_files::setup()
{
	std::vector<std::string> files = {};
	std::vector<std::string> paths = 
	{
		"Maps",
		"Maps/season1",
		"Maps/season2",
	};
	for (auto b : paths)
	{
		for (const auto& a : std::filesystem::directory_iterator(b))
		{
			if (a.path().string().find(".") == std::string::npos) continue;
			files.push_back(a.path().string());
			sdk::util::c_log::Instance().duo("found file: %s\n", a.path().string().c_str());
		}
	}
	this->files = files;
}
