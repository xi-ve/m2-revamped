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

		namespace hooks
		{
			typedef bool(__fastcall* t_IsPlayerAttacking)(uint32_t);
			extern t_IsPlayerAttacking o_IsPlayerAttacking;
			extern bool __fastcall f_IsPlayerAttacking(uint32_t base);
		}

		class c_waithack : public s<c_waithack>
		{
		private:
			void*							range, *targets, *speed, *toggle, *metins, *mobs_, *anchor, *on_attack, *bp_on_attack, *boost;
		private:
			ULONGLONG						timeout = 0, attack_timeout = 0;
		private:
			uint32_t						main_actor = 0;
			std::vector<s_mobs_area_point>	mobs;
		private:
			void							force_position(float x, float y);			
			bool							should_attack(uint32_t a);
			bool							populate();
			void							selective_attack();
		public:
			void							interpolate_to_pos(vec from, vec to);
		public:
			int								get_range();
			int								get_targets();
			int								get_speed();
			int								get_toggle();
			int								get_anchor();
			int								get_bp_on_attack();
			int								get_boost();
		private:
			int								get_mobs();
			int								get_metins();
			int								get_on_attack();
		public:
			void							setup();
			void							work();
		};
	}
}