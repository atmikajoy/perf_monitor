#ifndef PERF_SERVER_H_INCLUDED
#define PERF_SERVER_H_INCLUDED

#include "TCPServer.h"

namespace perf_monitor
{
	struct perf_server : public CTCPServer
	{
		perf_server();
		~perf_server();

		const std::string& port() const { return m_strPort; }

		void start_listening();
		void initiate_shutdown();
		static int recv_req(Socket connect_socket, char* data_buff, std::size_t sz);
		static void listener_thread(perf_server* server);
		static void client_thread( Socket connected_socket );
		static void trim_thread(perf_server* server);

		static std::atomic<bool> stopping;
		static std::atomic<int> num_connections ;
	};
};

#endif // PERF_SERVER_H_INCLUDED