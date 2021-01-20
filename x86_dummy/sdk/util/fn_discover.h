#pragma once
#include <inc.h>
using namespace nlohmann;
namespace sdk
{
	namespace util
	{
		struct s_mem
		{
			s_mem(uint32_t s, uint32_t e)
			{
				start = s;
				end = e;
			}
			uint32_t start;
			uint32_t end;
			std::unordered_map<uint32_t, std::vector<std::string>> listing;
			bool done = false;
		};
		extern void worker_thread(s_mem* mem);
		namespace json_fn_discover
		{
			struct s_info_entry
			{
				s_info_entry()
				{
					address = 0;
					strings = {};
				}
				s_info_entry(uint32_t a, std::vector<std::string> b)
				{
					address = a;
					strings = b;
				}
				uint32_t					address = 0;
				std::vector<std::string>	strings = {};
				NLOHMANN_DEFINE_TYPE_INTRUSIVE(s_info_entry, address, strings);
			};
		}
		typedef std::vector<json_fn_discover::s_info_entry> t_list;
		class c_fn_discover : public s<c_fn_discover>
		{
		private:
			bool			should_gen_fn_list = true;
		public:
			bool			is_ascii(const std::string& in);
		private:
			std::ifstream	fstream;
			std::ofstream	ofstream;
		private:
			const char*		get_exe_path();
			uint32_t		get_crc();
		private:
			const char*		file_name = "M2++_DYNAMICS.DB";
			void			save_db();
			void			load_db();
		private:
			t_list			fns;
			t_list			fns_py;
			bool			text_section();
			bool			data_section();
		public:
			uint32_t		get_fn_py(const char* fn_name);
			uint32_t		get_fn(const char* fn_str_ref);
		public:
			void			setup();
		};
	}
}