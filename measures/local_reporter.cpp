#include "local_reporter.h"
#include "history.h"

namespace perf_monitor
{

    void local_reporter::send(const counter& cntr)
    {
        history::push(cntr);
    }
}