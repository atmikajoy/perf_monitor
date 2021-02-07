#ifndef REMOTE_REPORTER_H_INCLUDED
#define REMOTE_REPORTER_H_INCLUDED

#include "reporter.h"
#include <string>
#include "TCPClient.h"

namespace perf_monitor
{
	struct remote_reporter : reporter
	{
		remote_reporter(const std::string& address, const std::string& port);
		~remote_reporter();

		virtual void send(const counter& cntr) override;

		// std::string address;
		// std::string port;
		CTCPClient tcp_client;
		bool connected;
	};
}

#endif // REMOTE_REPORTER_H_INCLUDED