#include "perf_server.h"
#include <iostream>
#include "history.h"
#include "nw_message.h"
#include "request.h"
#include <thread>
#include <iostream>

namespace perf_monitor
{
	namespace
	{
		const auto logger = [](const std::string& msg)
		{
			std::clog << "perf_server log: " << msg << '\n' << std::flush;
		};

		constexpr char port_number[] = "12345";

		std::vector<counter> filter( std::vector<history::cref_counter> cntrs, const request& req)
		{
			std::vector<counter> result;
			for ( const counter& c : cntrs)
				if (c.sl_no >= req.start_sl_no && c.time >= req.start_time)
					result.push_back(std::move(c));
			return result;
		}

		std::vector<counter> process_request(const request& req)
		{
			if (req.name == "*") return filter(history::peek(), req);

			if( req.type == request::BY_NAME ) 
				return filter(history::peek_name(req.name), req);
			else return filter(history::peek_srce(req.name), req);
		}
	}

	std::atomic<bool> perf_server::stopping{ false };
	std::atomic<int> perf_server::num_connections{ 0 };

	perf_server::perf_server() : CTCPServer(logger, port_number )
	{

	}

	perf_server::~perf_server()
	{
		while (num_connections > 0) std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "no active connections. perf_server::~perf_server() done\n";
	}

	void perf_server::start_listening() 
	{ 
		std::thread(listener_thread, this).detach() ;  
		std::thread(trim_thread, this).detach();
	}

	void perf_server::initiate_shutdown() { stopping = true; }

	void perf_server::listener_thread(perf_server* server)
	{
		while (!stopping)
		{
			Socket socket;
			if (server->Listen(socket))
			{
				++num_connections;
				std::thread(client_thread, socket).detach();
			}
		}
	}

	int perf_server::recv_req(Socket connect_socket,char* data_buff,std::size_t sz)
	{

		int tries = 0;
		std::size_t total = 0;
		while(total<sz)
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

				if ((nRecvd<0) & ENABLE_LOG)
					throw("[TCPServer][Error] Socket error in call to recv.");
				break;
			}
			total += nRecvd;

		} 

		return total;
	}
	

	void perf_server::client_thread(Socket connected_socket)
	{
		char data_buff[ request::STRING_LENGTH ]{};

		bool connected = true;
		
		while ( !stopping && connected && recv_req(connected_socket, data_buff, request::STRING_LENGTH) == request::STRING_LENGTH)
		{
			const auto request = request::from_string(data_buff);
			std::cout << "recd request on socket " << connected_socket << " : " << request.to_string() << '\n';
			const auto cntrs = process_request(request);
			for (const counter& cntr : cntrs )
			{
				const auto reply = reply::to_string(cntr);
				if (send(connected_socket, reply.c_str(), reply::STRING_LENGTH, 0) == SOCKET_ERROR)
				{
					connected = false;
					break;
				}
			}

			if ( send(connected_socket, reply::eot_msg(), reply::STRING_LENGTH, 0) == SOCKET_ERROR ) 
			{
				connected = false;
			}
	}

		if (shutdown(connected_socket, SD_RECEIVE) != SOCKET_ERROR)
			closesocket(connected_socket);
		std::cout << "socket connection on " << connected_socket << " is closed\n";
		--num_connections;

	}

	void perf_server::trim_thread(perf_server* server)
	{
		static const std::size_t trim_sz = 100'000;
		static const std::chrono::minutes trim_period(1 /* 5 */ );
		while (!stopping)
		{
			history::trim_size(trim_sz);
			std::this_thread::sleep_for(trim_period);
		}
	}

}
