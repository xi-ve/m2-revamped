#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		class c_log : public s<c_log>
		{
		private:
			const char*		log_file = "M2++_LOG.TXT";
			std::ofstream	fstream;
			std::wofstream	wfstream;
			void			to_console(const char* in);
			void			to_console(const wchar_t* in);
			void			to_file(const char* in);
			void			to_file(const wchar_t* in);
		public:
			void			setup();
			const char*		string(const char* in, ...);
			void			print(const char* in, ...);
			void			log(const char* in, ...);
			void			duo(const char* in, ...);
			void			duo(const wchar_t* in, ...);
		};
	}
}