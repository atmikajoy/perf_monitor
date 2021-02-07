#include "counter_names.h"

namespace perf_monitor
{
	//// COUNTER NAMES ////
	namespace counter_names
	{
		counter::type_t counter_type(std::string_view name)
		{
			if (name == CPU || name == MEMINUSE_PERCENT) return counter::PERCENTAGE;
			if (name == INTERRUPTS_PER_SEC || name == CONTEXT_SWITCHES_PER_SEC) return counter::RATE;
			return counter::NUMBER;
		}
	}
}
