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
		typedef std::vector<uint32_t>						t_addrs;
		class c_fn_discover : public s<c_fn_discover>
		{
		public:
			std::string		server_name = "";
		public:
			void			get_server();
		private:
			bool			should_gen_fn_list = false;
			bool			should_gen_advanced_str_refs = false;
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
			const char*		py_file_name = "M2++_PY_DYNAMICS.DB";
			void			save_fn_db();
			void			load_fn_db();
			void			save_py_fn_db();
			void			load_py_fn_db();
		private:
			t_list			fns;
			t_list			fns_py;
			t_list			offs_singletons;
			bool			text_section();
			bool			data_section();
			bool			singletons();
		public:
			bool			is_python_fn(uint32_t address);
			uint32_t		get_fn_py(const char* fn_name);
			t_addrs			get_adr_str(const char* ref, int max_strings);
			t_addrs			get_adr_str(const char* ref);
			uint32_t		get_fn(const char* fn_str_ref);
			uint32_t		discover_fn(uint32_t origin, size_t approx_size_min, size_t approx_size_max, size_t approx_calls = 0/*min cnt*/, size_t approx_off_movs = 0/*min cnt*/, bool no_calls_inside = false, bool no_off_push_inside = false, bool skip_py_exports = true, bool shoul_reverse_calls = false, bool should_include_jmp = false, uint32_t not_equal_to = 0);
		public:
			void			add_singleton(uint32_t address);
		public:
			void			setup();
		};
	}
}