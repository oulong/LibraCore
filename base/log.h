#ifndef __LIBRA_LOG_H__
#define __LIBRA_LOG_H__


#include "singleton.h"

#include <cstdio>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace Libra
{
	class Logger;
	enum
	{
		L_ALL_S = 0,
		L_DEBUG_S,
		L_INFO_S,
		L_WARN_S,
		L_ERROR_S,
		L_NONE_S
	};

	class LogImpl
	{
	public:
		virtual void log(int level, const std::string& s) = 0;
	};

	class Logger
	{
		enum {
			LOG_RECORD_MAX_SIZE = 4800,
		};
	public:
		Logger() 
			: core_(nullptr)
			, level_(L_ALL_S)
		{
		}
		
		void set_core(LogImpl* ins)
		{
			core_ = ins;
		}

		void set_level(int level)
		{
			level_ = level;
		}

		template<typename ... Args>
		void write(int level, const std::string& function, int line, const char* format, Args ... args)
		{
			if (level < level_) return;
			
			char buf[LOG_RECORD_MAX_SIZE] = { 0 };
			int p = 0;
#ifdef __WIN32
			p += _sprintf_p(buf + p, sizeof(buf) - p, "[%s:%d]", function.c_str(), line);
#else
			p += snprintf(buf + p, sizeof(buf) - p, "[%s:%d]", function.c_str(), line);
#endif	
			
#ifdef __WIN32
			_sprintf_p(buf + p, sizeof(buf) - p, format, args ...);
#else
			snprintf(buf + p, sizeof(buf) - p, format, args ...);
#endif
			if (core_)
			{
				core_->log(level, std::string(buf));
			}
			else
			{
				printf("%s\n", buf);
			}
		}
	private:
		LogImpl* core_;
		int		 level_;
	};

#ifdef __WIN32
#define debug_log(fmt, ...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define info_log(fmt, ...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define warn_log(fmt, ...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define error_log(fmt, ...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#else
#define debug_log(fmt, args...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, ##args)
#define info_log(fmt, args...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, ##args)
#define warn_log(fmt, args...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, ##args)
#define error_log(fmt, args...) Singleton<Logger>::instance()->write(L_DEBUG_S, __FUNCTION__, __LINE__, fmt, ##args)
#endif

}

#endif