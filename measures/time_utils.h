#ifndef TIME_UTILS_H_INCLUDED
#define TIME_UTILS_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <ctime>
#include <string>

namespace perf_monitor
{
	using clock = std::chrono::system_clock;
	using time_point = clock::time_point ;

	time_point start_time();

	inline time_point now() { return clock::now(); }

	unsigned long long millisecs( time_point tpt = now() ); // since start_time()

	inline std::time_t to_time_t( time_point tpt = now() ) 
	{ return clock::to_time_t(tpt); }

	std::tm calendar_time(time_point tpt = now());

	std::string str_time(time_point tpt = now());
	std::string str_time( std::time_t t );
}

#endif // TIME_UTILS_H_INCLUDED
