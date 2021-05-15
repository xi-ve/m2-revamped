#pragma once
//#pragma pack(8)
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
			void*							range, *targets, *speed, *toggle, *metins, *mobs_, *anchor, *on_attack, *bp_on_attack, *boost, *player,*bow_mode,*skill, *minatk ,*magnet;
			int last_tick = 0;
			int timer = 100;
		private:
			ULONGLONG						timeout = 0, attack_timeout = 0;
		private:
			uint32_t						main_actor = 0;
			std::vector<s_mobs_area_point>	mobs;
		public:
			int								flag_num = 0;
		private:
			void							force_position(float x, float y);
			bool							Send(uint32_t netbase, int len, const void* pSrcBuf);
			bool							should_attack(uint32_t a);
			bool							populate();
			void							selective_attack();
		public:
			void							interpolate_to_pos(vec from, vec to);
			void							interpolate_to_pos_with_mob(uint32_t netbase, DWORD mob, vec from, vec to);

		public:
			int								get_range();
			int								get_targets();
			int								get_speed();
			int								get_toggle();
			int								get_anchor();
			int								get_bp_on_attack();
			int								get_boost();
			int								get_bow_mode();
			int								get_skill();
			int								get_minatk();
			int								get_magnet();
		private:
			int								get_mobs();
			int								get_metins();
			int								get_player();
			int								get_on_attack();
		public:
			void							setup();
			void							work();
		};
	}
}

