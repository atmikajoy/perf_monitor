#include "remote_reporter.h"
#include "nw_message.h"
#include <iostream>

namespace perf_monitor
{
    namespace
    {
        const auto logger = [] (const auto & msg){ std::cout << "remote_reporter: " << msg << '\n'; };
    }

    remote_reporter::remote_reporter(const std::string& address, const std::string& port)
        : tcp_client(logger)
    {
        connected = tcp_client.Connect(address, port);
    }

    remote_reporter::~remote_reporter()
    {
        if (connected) tcp_client.Disconnect();
    }

    void remote_reporter::send(const counter& cntr)
    {
        if (connected) tcp_client.Send(nw_message(cntr).text);
    }
}