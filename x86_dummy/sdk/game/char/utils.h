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
			};
		}
	}
}