#ifndef COUNTER_NAMES_H_INCLUDED
#define COUNTER_NAMES_H_INCLUDED

#include <string_view>
#include "counter.h"

namespace perf_monitor
{
	//// COUNTER NAMES ////
	namespace counter_names
	{
		constexpr std::string_view CPU = "\\Processor(_Total)\\% Processor Time"; // % of cpu in use
		constexpr std::string_view MEMINUSE_BYTES = "\\Memory\\Committed Bytes"; // mem in use measured in bytes
		constexpr std::string_view MEMAVAIL_BYTES = "\\Memory\\Available Bytes"; // mem available measured in bytes
		constexpr std::string_view MEMAVAIL_KB = "\\Memory\\Available KBytes"; // mem avail in kilobytes
		constexpr std::string_view MEMAVAIL_MB = "\\Memory\\Available MBytes"; // mem avail in megabytes
		constexpr std::string_view MEMINUSE_PERCENT = "\\Memory\\% Committed Bytes In Use"; // % of mem in use
		constexpr std::string_view MEMLIMIT_BYTES = "\\Memory\\Commit Limit"; // commit limit on memory in bytes
		constexpr std::string_view DISK_IO_BYTES = "\\PhysicalDisk(_Total)\\Disk Bytes/sec";
		constexpr std::string_view POWER_METER = "\\Power Meter(Power Meter (0))\\Power"; // power consumption milli watts
		constexpr std::string_view INTERRUPTS_PER_SEC = "\\Processor(_Total)\\Interrupts/sec"; // interrupt service per sec
		constexpr std::string_view NUM_PROCESSES = "\\Objects\\Processes"; //#processes running on system 
		constexpr std::string_view NUM_THREADS = "\\Objects\\Threads"; // #threads running on system 
		constexpr std::string_view CONTEXT_SWITCHES_PER_SEC = "\\Thread(_Total/_Total)\\Context Switches/sec"; // #context swiotches per second
	
		counter::type_t counter_type(std::string_view name);

		std::string description( const std::string& name, bool ucase = false );
	}

}

#endif // COUNTER_NAMES_H_INCLUDED