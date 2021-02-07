#ifndef SEND_PERF_H_INCLUDED
#define SEND_PERF_H_INCLUDED

#include<string>

namespace perf_monitor
{
	struct send_perf
	{

		send_perf(std::string_view da);
		void send_thread();


		std::string dest_address; 
	};
}

#endif // !SEND_PERF_H_INCLUDED
