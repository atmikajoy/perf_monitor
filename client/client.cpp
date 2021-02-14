
#include "TCPClient.h"
#include <iostream>
#include "nw_message.h"
#include "counter_names.h"
#include "request.h"
#include <vector>

int main()
{
    using namespace perf_monitor;

    unsigned int update_interval_secs = 10;
    std::cout << "update_interval_secs? ";
    std::cin >> update_interval_secs;
    if (update_interval_secs < 5) update_interval_secs = 5;
    const auto update_interval = std::chrono::seconds(update_interval_secs);

    const auto log = [](const auto& msg)
    { std::cout << msg << '\n'; };

    const std::string address = "LAPTOP-EBCTSM8H";
    const std::string port = "12345";

    CTCPClient tcp_client(log);
    if (!tcp_client.Connect(address, port))
    {
        std::cout << "connect failed\n";
        std::cout << "client exiting. press enter... ";
        std::cin.get();
        return 1;
    }

    unsigned long long sl_no = 0;
    const std::string name = std::string(counter_names::CPU);

    while (true)
    {
        request requests[]
        {
            { request::BY_NAME, std::string(counter_names::CPU), sl_no, 0 },
            { request::BY_NAME, std::string(counter_names::MEMINUSE_BYTES), sl_no, 0 },
            { request::BY_NAME, std::string(counter_names::CONTEXT_SWITCHES_PER_SEC), sl_no, 0 },

        };

        std::cout << "\n\n";

        for( const auto& req : requests )
        { 

            std::cout << "\nsending request " << req.to_string() << '\n';

            std::string last_reply;

            if (tcp_client.Send(req.to_string()))
            {
                int cnt = 0;
                char buffer[reply::STRING_LENGTH]{};
                while (tcp_client.Receive(buffer, reply::STRING_LENGTH) == reply::STRING_LENGTH)
                {
                    if (buffer != reply::eot)
                    {
                        const counter cntr = nw_message::text_to_counter(buffer);
                        sl_no = cntr.sl_no + 1;
                        // std::cout << "recd: " << cntr.to_string() << '\n';
                        last_reply = cntr.to_string_brief();
                        std::cout << ++cnt << ". " << last_reply << '\n';
                    }

                    else
                    {
                        // std::cout << "recd eot\n";
                        //std::cout << "recd: " << last_reply << '\n';
                        break;
                    }
                }
            }
            else break;

        }
        std::this_thread::sleep_for(update_interval);

    }

    tcp_client.Disconnect();

    std::cout << "client exiting. press enter... ";
    std::cin.get();

}
