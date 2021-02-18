#include "perf_ssl_server.h"
#include"request.h"
#include<vector>
#include "history.h"

namespace perf_monitor 
{
	namespace 
	{
		const auto logger = [](const std::string& msg)
		{
			std::clog << "perf_server log: " << msg << '\n' << std::flush;
		};

		char port_number[] = "12345";

		std::vector<counter> filter(std::vector<history::cref_counter> cntrs, const request& req)
		{
			std::vector<counter> result;
			for ( const counter& c : cntrs)
				if (c.sl_no >= req.start_sl_no && c.time >= req.start_time)
					result.push_back(c);
			return result;
		}

		std::vector<counter> process_request(const request& req)
		{
			if (req.name == "*") return filter(history::peek(), req);

			if (req.type == request::BY_NAME)
				return filter(history::peek_name(req.name), req);
			else return filter(history::peek_srce(req.name), req);
		}

		
	}

	std::atomic<bool> perf_ssl_server::stopping{ false };
	std::atomic<int> perf_ssl_server::num_connections{ 0 };

	perf_ssl_server::perf_ssl_server() : CTCPSSLServer(logger, port_number)
	{

	}

	perf_ssl_server::~perf_ssl_server()
	{
		while(num_connections>0) std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "No active connections\n";
	}

	void perf_ssl_server::initiate_shutdown() { stopping = true; }

	void perf_ssl_server::start_listening()
	{
		std::thread(listener_thread, this).detach();
		std::thread(trim_thread, this).detach();
	}

	void perf_ssl_server::listener_thread(perf_ssl_server* server)
	{
		server->SetSSLCertFile("C:\\perf_mon_cert\\cert.pem");
		server->SetSSLKeyFile("C:\\perf_mon_cert\\key.pem");

		while (!stopping)
		{
			SSLSocket socket; 
			if (server->Listen(socket))
			{
				++num_connections; 
				std::thread(client_thread, std::move(socket), server).detach();
			}	
		}
	}

	int perf_ssl_server::recv_req(SSLSocket& connect_socket, char* data_buff, std::size_t sz)
	{

		int tries = 0;
		std::size_t total = 0;
		while (total < sz)
		{
			int nRecvd = SSL_read(connect_socket.m_pSSL, data_buff + total, sz - total);
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

	void perf_ssl_server::client_thread(SSLSocket connected_socket, perf_ssl_server* server)
	{
		char data_buff[request::STRING_LENGTH]{};

		bool connected = true;

		while (!stopping && connected && recv_req(connected_socket, data_buff, request::STRING_LENGTH) == request::STRING_LENGTH)
		{
			const auto request = request::from_string(data_buff);
			std::cout << "recd request on socket " << connected_socket.m_SockFd << " : " << request.to_string() << '\n';
			const auto cntrs = process_request(request);
			for (const counter& cntr : cntrs)
			{
				const auto reply = reply::to_string(cntr);
				if (SSL_write(connected_socket.m_pSSL, reply.c_str(), reply::STRING_LENGTH) == SOCKET_ERROR)
				{
					connected = false;
					break;
				}
			}

			if (SSL_write(connected_socket.m_pSSL, reply::eot_msg(), reply::STRING_LENGTH) == SOCKET_ERROR)
			{
				connected = false;
			}
		}

		if (server->Disconnect(connected_socket) == false)
			std::cout << "socket connection on " << connected_socket.m_SockFd << " is closed\n";
		--num_connections;

	}

	void perf_ssl_server::trim_thread(perf_ssl_server* server)
	{
		static const std::size_t trim_sz = 100'000;
		static const std::chrono::minutes trim_period(1 /* 5 */);
		while (!stopping)
		{
			history::trim_size(trim_sz);
			std::this_thread::sleep_for(trim_period);
		}
	}

}