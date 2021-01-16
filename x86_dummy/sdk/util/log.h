#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		class c_log : public s<c_log>
		{
		private:
			const char*		log_file = "LOG_M2++.TXT";
			std::ofstream	fstream;
			void			to_console(const char* in);
			void			to_file(const char* in);
		public:
			void			setup();
			const char*		string(const char* in, ...);
			void			print(const char* in, ...);
			void			log(const char* in, ...);
			void			duo(const char* in, ...);
		};
	}
}