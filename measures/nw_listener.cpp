#include "nw_listener.h"
#include <iostream>
#include "history.h"
#include "nw_message.h"
#include <thread>

namespace perf_monitor
{
	namespace
	{
		const auto logger = [](const std::string& msg)
		{
			std::clog << "nw_listener log: " << msg << '\n' << std::flush;
		};

		constexpr char port_number[] = "23456";

	}

	std::atomic<bool> nw_listener::stopping{ false };
	std::atomic<int> nw_listener::num_connections{ 0 };

	nw_listener::nw_listener() : CTCPServer(logger, port_number)
	{

	}

	nw_listener::~nw_listener()
	{
		// CTCPServer::;
		// while (num_connections > 0) std::this_thread::sleep_for(std::chrono::seconds(1));
		// std::cout << "no active connections. perf_server::~perf_server() done\n";
	}

	void nw_listener::start_listening()
	{
		std::thread(listener_thread, this).detach();
	}

	void nw_listener::initiate_shutdown() { stopping = true; }

	void nw_listener::listener_thread(nw_listener* listener )
	{
		while (!stopping)
		{
			Socket socket;
			if (listener->Listen(socket))
			{
				++num_connections;
				std::thread(client_thread, socket).detach();
			}
		}
	}

	int nw_listener::recv_msg(Socket connect_socket, char* data_buff, std::size_t sz)
	{

		int tries = 0;
		std::size_t total = 0;
		while (total < sz)
		{
			int nRecvd = recv(connect_socket, data_buff + total, sz - total, 0);
			if (nRecvd == 0) break;
			if ((nRecvd < 0) && (WSAGetLastError() == WSAENOBUFS))
			{
				if ((tries++ < 1000))
				{
					Sleep(1);
					continue;
				}

				if ((nRecvd < 0) & ENABLE_LOG)
					throw("[TCPServer][Error] Socket error in call to recv.");
				break;
			}
			total += nRecvd;

		}

		return total;
	}


	void nw_listener::client_thread(Socket connected_socket)
	{
		char data_buff[nw_message::FIXED_SIZE]{};

		while (!stopping && recv_msg(connected_socket, data_buff, nw_message::FIXED_SIZE) == nw_message::FIXED_SIZE)
		{
			history::push(nw_message::text_to_counter(data_buff));
		}

		if (shutdown(connected_socket, SD_RECEIVE) != SOCKET_ERROR)
			closesocket(connected_socket);
		--num_connections;

	}

}
