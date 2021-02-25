#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace json_pickup
		{
			struct s_item_config
			{
				std::vector<uint32_t>		item_blacklist = {};
				std::vector<uint32_t>		item_whitelist = {};
				bool						only_pick_blacklist = 0;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(s_item_config, item_blacklist, item_whitelist, only_pick_blacklist);
			};
		}
		class c_pickup : public s<c_pickup>
		{
		private:
			std::string					item_conf_file = "M2++_ITEM_CONF.TXT";			
			ULONGLONG					last_loot = 0;
			void*						toggle, *distance, *delay;
		public:
			json_pickup::s_item_config	item_conf = {};
		private:
			int							get_toggle();
			int							get_distance();
			int							get_delay();
		private:
			bool						should_loot(uint32_t vnum);
		public:
			bool						has_config();
			void						load(); 
			void						make_clean_conf();
			void						setup();
			void						save();
			void						clear_whitelist();
			void						clear_blacklist();
			bool						add_blacklist(uint32_t vnum);
			bool						add_whitelist(uint32_t vnum);
			void						unblacklist(uint32_t vnum);
			void						unwhitelist(uint32_t vnum);
			void						set_blacklist_mode(bool mode);
			bool						is_whitelisted(uint32_t vnum);
			bool						is_blacklisted(uint32_t vnum);
			void						work();
		};
	}
}