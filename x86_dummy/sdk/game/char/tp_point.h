#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace chr
		{
			namespace json_tp_points
			{
				struct s_tp_point_cfg
				{
					s_tp_point_cfg(){}
					s_tp_point_cfg(int xa, int ya, int za, std::string name, std::string map)
					{
						point_name = name;
						map_name = map;
						x = std::to_string(xa);
						y = std::to_string(ya);
						z = std::to_string(za);
						pos = sdk::util::math::c_vector3( (float)xa, (float)ya, (float)za );
					}
					std::string					map_name = "";
					std::string					point_name = "";
					std::string					x = "";
					std::string					y = "";
					std::string					z = "";
					sdk::util::math::c_vector3	pos = {};
					NLOHMANN_DEFINE_TYPE_INTRUSIVE(s_tp_point_cfg, point_name, x, y, z, map_name);
				};
			}
			typedef std::vector<json_tp_points::s_tp_point_cfg>	tp_points;
			typedef std::vector<std::string>					str_vec;
			class c_tp_point : public s<c_tp_point>
			{
			private:
				void*		last_point;
				std::string	file_name = "M2++_TP_POINTS.DB";
			public:
				std::string	get_last_point();
			private:
				tp_points	points = {};
			public:
				void		add(float x, float y, float z, std::string name);
				tp_points	get_points() { return this->points; };
				str_vec		get_names();
				vec			get_point(std::string name);
				std::string	get_point_map(std::string name);
				void		load();
				void		save();
			public:				

				void		setup();
				void		work(std::string point);
			};
		}
	}
}