#ifndef PERFMON_H_INCLUDED
#define PERFMON_H_INCLUDED

#include<string>
#include <map>
#include <pdh.h>
#include <pdhmsg.h>
#include <vector>
#include <string_view>

namespace perf_monitor
{
	struct perf_measures
	{
		perf_measures( std::initializer_list< std::string_view > counter_list );
		perf_measures(); // measure all counters
		~perf_measures();
		perf_measures& operator= (const perf_measures&) = delete;
		perf_measures& operator= (perf_measures&&) = delete;

		std::vector< std::string_view > GetCounters();
		std::map< std::string_view, double > GetCounterValues();
		bool AddCounter(const std::string& name);
		bool RemoveCounter(const std::string& name);
		double GetCounterValue(const std::string& name);
		bool CollectQueryData();

		private:
			std::map< std::string, HCOUNTER > counter_map;
			HCOUNTER find(const std::string& name);
			HQUERY m_hQuery; // the query to the PDH
			double GetCounterValue(HCOUNTER h);
	};
	
}




#endif // !PERFMON_H_INCLUDED
