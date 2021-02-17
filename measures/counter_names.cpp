#include "counter_names.h"
#include <map>

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

		std::string description(const std::string& name, bool ucase)
		{
			static const std::map< std::string, std::string > descr =
			{
				{ std::string(CPU), "% of cpu in use" },
				{std::string(MEMINUSE_BYTES), "memory in use measured in bytes" },
				{std::string(MEMAVAIL_BYTES), "memory available measured in bytes"},
				{std::string(MEMAVAIL_KB),"memory available in kilobytes"},
				{std::string(MEMAVAIL_MB), "memory available in megabytes" },
				{std::string(MEMINUSE_PERCENT),"% of memory in use"},
				{std::string(MEMLIMIT_BYTES), "commit limit on memory in bytes"},
				{std::string(DISK_IO_BYTES),"total disk i/o bytes/sec"},
				{std::string(POWER_METER),"power consumption milli watts"},
				{std::string(INTERRUPTS_PER_SEC),"number of interrupts serviced per second"},
				{std::string(NUM_PROCESSES),"number of processes running on system"},
				{std::string(NUM_THREADS),"number of threads running on system"},
				{std::string(CONTEXT_SWITCHES_PER_SEC),"number of context switches per second"}
			};

			auto iter = descr.find(name);
			if (iter == descr.end()) return name;

			std::string result = iter->second;
			if (ucase) for (char& c : result) c = std::toupper(static_cast<unsigned char>(c));
			return result;
		

		}

		const std::vector<std::string>& list()
		{
			static const std::vector<std::string> lst
			{
				std::string(CPU),
				std::string(MEMINUSE_BYTES),
				std::string(MEMAVAIL_BYTES),
				std::string(MEMAVAIL_KB),
				std::string(MEMAVAIL_MB),
				std::string(MEMINUSE_PERCENT),
				std::string(MEMLIMIT_BYTES),
				std::string(DISK_IO_BYTES),
				std::string(POWER_METER),
				std::string(INTERRUPTS_PER_SEC),
				std::string(NUM_PROCESSES),
				std::string(NUM_THREADS),
				std::string(CONTEXT_SWITCHES_PER_SEC)
			};

			return lst;
		}
	}
}
