#include"perfmon.h"
#include<vector>
// #include <afxtempl.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <array>
#include<algorithm>
#include <stdexcept>
#include <map>

namespace perf_monitor
{
	HCOUNTER perf_measures::find(const std::string& name)
	{
		auto iter = counter_map.find(name);
		return iter != counter_map.end() ? iter->second : nullptr ;
	}

	double perf_measures::GetCounterValue(HCOUNTER hCounter )
	{
		if (hCounter == nullptr) return -1;

		PDH_FMT_COUNTERVALUE pdhFormattedValue;

		// get the value from the PDH
		const auto res = PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, NULL, &pdhFormattedValue);
		if (res != ERROR_SUCCESS) return -1;

		// test the value for validity
		if (pdhFormattedValue.CStatus != ERROR_SUCCESS) return -1;

		return pdhFormattedValue.doubleValue;
	}

	double perf_measures::GetCounterValue(const std::string& name )
	{
		return GetCounterValue( find(name) );
	}

	perf_measures::perf_measures( std::initializer_list< std::string_view > counter_list )
	{
		if( PdhOpenQuery(NULL, 1, &m_hQuery) != ERROR_SUCCESS )
			throw std::runtime_error("init failed");
        
		bool failed = false;
		for (const auto& cntr : counter_list) if( !AddCounter( std::string(cntr) ) ) failed = true ;
		if( failed ) throw std::runtime_error("atleast one AddCounter failed: data not collected");

		CollectQueryData();
	}

	perf_measures::~perf_measures()
	{
		PdhCloseQuery(&m_hQuery);
	}

	std::vector< std::string_view > perf_measures::GetCounters()
	{
		std::vector< std::string_view > counters;
		for (const auto& [name, h] : counter_map) counters.push_back(name);
		return counters;
	}

	std::map< std::string_view, double > perf_measures::GetCounterValues()
	{
		std::map< std::string_view, double > result;

		if (!counter_map.empty())
		{
			CollectQueryData();
			for (const auto& [name, h] : counter_map) result.emplace(name, GetCounterValue(h));
		}

		return result;
	}

	bool perf_measures::AddCounter(const std::string& name)
	{
		if (find(name) != nullptr) return false; // this counter was already added earlier

		HCOUNTER hcntr = nullptr;
		// add to current query
		if (PdhAddCounter(m_hQuery, name.c_str(), 0, &hcntr) == ERROR_SUCCESS)
		{
			counter_map[name] = hcntr;
			return true;
		}

		return false;
	}

	bool perf_measures::RemoveCounter(const std::string& name)
	{
		auto hcntr = find(name);
		if (hcntr == nullptr) return false; 

		if( PdhRemoveCounter( hcntr ) == ERROR_SUCCESS )
		{
			counter_map.erase(name);
			return true;
		}

		return false ;
	}

	bool perf_measures::CollectQueryData()
	{
		if (PdhCollectQueryData(m_hQuery) != ERROR_SUCCESS) return false;
		return true;
	}
}