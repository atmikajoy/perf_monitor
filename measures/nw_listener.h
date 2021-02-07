#ifndef NW_LISTENER_H_INCLUDED
#define NW_LISTENER_H_INCLUDED

#include "TCPServer.h"

namespace perf_monitor
{
	struct nw_listener : public CTCPServer
	{
		nw_listener();
		~nw_listener();

		const std::string& port() const { return m_strPort; }

		void start_listening();
		void initiate_shutdown();
		static int recv_msg(Socket connect_socket, char* data_buff, std::size_t sz);
		static void listener_thread(nw_listener* listener);
		static void client_thread(Socket connected_socket);

		static std::atomic<bool> stopping;
		static std::atomic<int> num_connections;
	};
};

#endif // NW_LISTENER_H_INCLUDED