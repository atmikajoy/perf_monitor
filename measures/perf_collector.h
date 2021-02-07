#ifndef PERF_COLLECTER_H_INCLUDED
#define PERF_COLLECTER_H_INCLUDED

#include "counter_names.h"
#include <initializer_list>
#include <atomic>
#include "perfmon.h"
#include "reporter.h"

namespace perf_monitor
{
	struct perf_recorder
	{
		perf_recorder( std::initializer_list<std::string_view> counter_names, unsigned int collect_interval_secs, 
			           reporter& rptr );

		~perf_recorder();

		perf_recorder(const perf_recorder&) = delete;
		perf_recorder(perf_recorder&&) = delete;

		perf_measures measurer;

		static void collect_thread(perf_recorder* recorder);
		static std::atomic<bool> done;
		std::chrono::seconds collect_interval;
		reporter& reprtr;
	};
}

#endif // PERF_COLLECTER_H_INCLUDED
