#pragma comment( lib, "pdh.lib" )

#include "counter.h"
#include <iostream>
#include "time_utils.h"
#include "nw_message.h"
#include <iomanip>
#include <thread>
#include <chrono>
#include "history.h"
#include "request.h"
#include "perf_server.h"
#include "TCPClient.h"
#include <limits>
#include <cstdio>
#include"perfmon.h"
#include"counter_names.h"
#include <Pdh.h>
#include <typeinfo>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "local_reporter.h"
#include "perf_collector.h"
#include "nw_listener.h"
#include "perf_server.h"
#include"perf_ssl_server.h"

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")


int main()
{
	using namespace perf_monitor;
	using namespace counter_names;

	local_reporter reporter;
	perf_recorder recorder(5,reporter); // ({ CPU, MEMINUSE_BYTES, CONTEXT_SWITCHES_PER_SEC }, 5, reporter);

	// nw_listener listener;
	// listener.start_listening();
	perf_ssl_server server;
	server.start_listening();

	std::cout << "press enter to start shut down of server\n";
	std::cin.get();
	recorder.done = true;
	// server.stopping = true;
	server.initiate_shutdown();
	// listener.stopping = true;
	// std::cin.get();
	// listener.Disconnect();
	std::cout << "waiting for 100 seconds..." << std::flush;
	for (int i = 0; i < 2 ; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
		std::cout << '.' << std::flush;
	}
	std::cout << " done\n";
}

/*
int main()
{
	{
		using namespace perf_monitor;
		using namespace perf_monitor::counter_names;

		perf_monitor::perf_measures processor( { CPU, INTERRUPTS_PER_SEC } );

		perf_monitor::perf_measures disk( { MEMINUSE_BYTES, DISK_IO_BYTES } );

		perf_monitor::perf_measures power({ POWER_METER, NUM_PROCESSES, NUM_THREADS } );

		Sleep(500);

		{
			std::cout << "ok\n";


			double total_cpu_usage = 0; 
			for (int i = 0; i < 20; ++i)
			{
				Sleep(1000);
				auto processor_counters = processor.GetCounterValues();
				auto disk_counters = disk.GetCounterValues();
				auto power_counters = power.GetCounterValues();

				auto cpu_usage = processor_counters[CPU];
				total_cpu_usage += cpu_usage;
				std::cout << std::fixed
					<< "MEMINUSE_BYTES: " << disk_counters[MEMINUSE_BYTES] << '\n'
					<< "CPU: " << cpu_usage << "%\n"
					<< "DISK_IO_BYTES: " << disk_counters[DISK_IO_BYTES] << '\n'
					<< "POWER_METER: " << power_counters[POWER_METER] << " milli watts\n"
					<< "INTERRUPTS_PER_SEC: " << processor_counters[INTERRUPTS_PER_SEC] << "/sec\n"
					<< "NUM_PROCESSES: " << power_counters[NUM_PROCESSES] << '\n'
					<< "NUM_THREADS: " << power_counters[NUM_THREADS] << '\n'
					<< "-------------------------------------------------------------------\n";
			}
			std::cout << "AVERAGE CPU USAGE = " << (total_cpu_usage / 20);
		}
		

	}
	

}
*/
