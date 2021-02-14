#include "time_utils.h"
#include<sstream>

namespace perf_monitor
{
	time_point start_time()
	{
		static const auto t_start = now();
		return t_start;
	}

	unsigned long long millisecs( time_point tpt )
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(tpt - start_time()).count();
	}

	std::tm calendar_time( time_point tpt )
	{
		auto temp_time_t = to_time_t(tpt);
		return (*localtime(&temp_time_t));
	}

	std::string str_time(time_point tpt)
	{
		return str_time( to_time_t(tpt) );
	}

	std::string str_time(std::time_t t)
	{
		const auto tm = *localtime(std::addressof(t));
		char temp_buffer[256];
		std::strftime(temp_buffer, sizeof(temp_buffer), "%d/%m/%Y %H:%M:%S", &tm);
		return temp_buffer;
	}
}