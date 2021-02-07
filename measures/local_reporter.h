#ifndef LOCAL_REPORTER_H_INCLUDED
#define LOCAL_REPORTER_H_INCLUDED

#include "reporter.h"

namespace perf_monitor
{
	struct local_reporter : reporter
	{

		virtual void send(const counter& cntr) override;
	};
}

#endif // LOCAL_REPORTER_H_INCLUDED