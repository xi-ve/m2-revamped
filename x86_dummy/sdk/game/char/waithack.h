#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		typedef sdk::util::math::c_vector3 vec;
		struct s_mobs_area_point
		{
			uint32_t				vid;
			std::vector<uint32_t>	mobs;
		};

		

		class c_waithack : public s<c_waithack>
		{
		private:
			void*							range, *targets, *speed, *toggle, *metins, *mobs_, *anchor;
		private:
			ULONGLONG						timeout = 0, attack_timeout = 0;
		private:
			uint32_t						main_actor = 0;
			std::vector<s_mobs_area_point>	mobs;
		private:
			void							force_position(float x, float y);
			void							interpolate_to_pos(vec from, vec to);
			bool							should_attack(uint32_t a);
			bool							populate();
			void							selective_attack();
		public:
			int								get_range();
			int								get_targets();
			int								get_speed();
			int								get_toggle();
			int								get_anchor();
		private:
			int								get_mobs();
			int								get_metins();
		public:
			void							setup();
			void							work();
		};
	}
}