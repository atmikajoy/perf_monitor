#ifndef OPENSSL
#define OPENSSL
#endif // OPENSSL

#include"TCPSSLClient.h"
#include "TCPClient.h"
#include <iostream>
#include "nw_message.h"
#include "counter_names.h"
#include "request.h"
#include <vector>
#include <iomanip>

std::vector<std::string> select_counters()
{
    static const auto& names = perf_monitor::counter_names::list();

    for (std::size_t i = 0; i < names.size(); ++i)
        std::cout << i + 1 << ". " << perf_monitor::counter_names::description(names[i]) << '\n';

    std::cout << "enter item numbers (enter -1 to end): ";
    std::vector<std::string> result;
    int n;
    while (std::cin >> n && n > 0 && n <= int(names.size()) ) result.push_back(names[n- 1]);
    return result;
}

int main()
{
    using namespace perf_monitor;

    unsigned int update_interval_secs = 10;
    std::cout << "update_interval_secs? ";
    std::cin >> update_interval_secs;
    if (update_interval_secs < 5) update_interval_secs = 5;
    const auto update_interval = std::chrono::seconds(update_interval_secs);
    
    std::vector<request> requests;
    unsigned long long sl_no = 0;
    for (std::string name : select_counters())
        requests.push_back({ request::BY_NAME, name, sl_no, std::time(nullptr) - update_interval_secs });
    

    const auto log = [](const auto& msg)
    { std::cout << msg << '\n'; };

    const std::string address = "LAPTOP-EBCTSM8H";
    const std::string port = "12345";

    CTCPSSLClient tcp_client(log);
    tcp_client.SetSSLCertFile("C:\\perf_mon_cert\\client_cert.pem");
    tcp_client.SetSSLKeyFile("C:\\perf_mon_cert\\client_key.pem");

    if (!tcp_client.Connect(address, port))
    {
        std::cout << "connect failed\n";
        std::cout << "client exiting. press enter... ";
        std::cin.get();
        return 1;
    }


    const std::string name = std::string(counter_names::CPU);



    while (true)
    {
        std::cout << "\n\n";

        unsigned long long curr_start_sl_no = sl_no;

        for( auto& req : requests )
        { 
            req.start_sl_no = curr_start_sl_no;
            std::cout << "\nsending request " << req.to_string() << '\t' 
                      << perf_monitor::counter_names::description(req.name) << '\n';

            std::string last_reply;

            if (tcp_client.Send(req.to_string()))
            {
                double moving_sum = 0;
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
                        ++cnt;
                        switch (cntr.type())
                        {
                        case counter::NUMBER:
                            moving_sum += std::get<counter::NUMBER>(cntr.info);
                            break;

                        case counter::PERCENTAGE:
                            moving_sum += std::get<counter::PERCENTAGE>(cntr.info);
                            break;

                        case counter::RATE:
                        {
                            const auto& r = std::get<counter::RATE>(cntr.info);
                            double cnt = double(r.count);
                            double secs = r.period_millisecs / 1000.0;
                            moving_sum += cnt / secs;
                        }
                        break;
                        }

                    }

                    else
                    {
                        if (cnt > 0)
                        {
                            // std::cout << "recd eot\n";
                            std::cout << "last reading: " << last_reply << '\n';
                            std::cout << "\t\t\taverage of " << cnt << " readings: " 
                                      << std::fixed << std::setprecision(2) << moving_sum / cnt << '\n' ;
                            // moving_sum = 0; cnt = 0;
                        }
                        else std::cout << "no measurements\n";
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
