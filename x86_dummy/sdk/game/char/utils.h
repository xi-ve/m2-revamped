#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace chr
		{
			struct s_actor
			{
				std::string					name = "";
				uint8_t						type = 0;
				uint32_t					vid = 0;
				uint32_t					adr = 0;
				sdk::util::math::c_vector3	position;
			};
			typedef sdk::util::math::c_vector3 vec;
			class c_char : public s<c_char>
			{
			private:
				std::map<uint32_t, uint32_t*>	alive_actors;
				std::list<uint32_t*>			dead_actors;
			public:
				void							setup();
				void							update();
				std::list<uint32_t*>			get_dead();
				std::map<uint32_t, uint32_t*>	get_alive();
			public:
				bool							use_alt_mode_list = 0;
				uint32_t						get_main_actor();
				uint32_t						get_graphic_thing(uint32_t instance_base);
				bool							is_dead_actor(uint32_t instance_base);
				uint32_t						get_type(uint32_t instance_base);
				vec								get_pos(uint32_t instance_base);
				void							set_pos(uint32_t instance_base,float x,float y,float z);

				float							get_distance(vec from, vec to);
				std::string						get_name(uint32_t instance_base);
				bool							is_attackable(uint32_t instance_base);
				uint32_t						get_vid(uint32_t instance_base);
				uint32_t						get_instance(uint32_t vid);
			};
		}
	}
}