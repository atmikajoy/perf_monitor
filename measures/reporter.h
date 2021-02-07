#ifndef REPORT_PERF_H_INCLUDED
#define REPORT_PERF_H_INCLUDED

#include "counter.h"

namespace perf_monitor
{
	struct reporter
	{
		virtual ~reporter() = default;

		virtual void send( const counter& cntr ) = 0;
	};
}

#endif // REPORT_PERF_H_INCLUDED