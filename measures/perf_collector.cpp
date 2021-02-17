#include "perf_collector.h"
#include <thread>
/*
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
*/
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

namespace perf_monitor
{
	namespace
	{
		std::string hostname()
		{
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2, 2), &wsaData);
			char name[256]{};
			gethostname(name, sizeof(name));
			WSACleanup();
			
			return name;
		}
	}

	perf_recorder::perf_recorder(std::initializer_list<std::string_view> counter_names, unsigned int collect_interval_secs,
		                           reporter& reprtr ) : measurer(counter_names), reprtr(reprtr)
	{
		std::cout << "perf_recorder: collect_interval == " << collect_interval_secs << " seconds\n";
		collect_interval = std::chrono::seconds(collect_interval_secs);
		std::thread(collect_thread, this).detach();
	}

	perf_recorder::perf_recorder(unsigned int collect_interval_secs, reporter& rptr) : reprtr(rptr)
	{
		std::cout << "perf_recorder: collect_interval == " << collect_interval_secs << " seconds\n";
		collect_interval = std::chrono::seconds(collect_interval_secs);
		std::thread(collect_thread, this).detach();
	}

	perf_recorder::~perf_recorder()
	{
		done = true ;
		std::this_thread::sleep_for(collect_interval);
	}

	void perf_recorder::collect_thread(perf_recorder* recorder)
	{
		recorder->measurer.CollectQueryData();
		static const std::string this_host = hostname();

		while (!recorder->done)
		{
			using ullong = unsigned long long;
			constexpr unsigned int period_ms = 1000; // one second

			for (const auto& [name, value] : recorder->measurer.GetCounterValues())
			{
				switch (counter_names::counter_type(name))
				{
				   case counter::NUMBER :
					   // history::push(counter(name, ullong(value), this_host )) ;
					   recorder->reprtr.send(counter(name, ullong(value), this_host));
					   break;

				   case counter::PERCENTAGE:
					   // history::push(counter(name, value, this_host ));
					   recorder->reprtr.send(counter(name, value, this_host));
					   break;
                   
				   default: // counter::RATE (all our rates are per second)
					   // history::push( counter(name, ullong(value), period_ms, this_host ) );
					   recorder->reprtr.send(counter(name, ullong(value), period_ms, this_host));
				}
			}

			std::this_thread::sleep_for(recorder->collect_interval);
		}
	}
	
	std::atomic<bool> perf_recorder::done{ false } ;

}