#ifndef PERF_SSL_SERVER_H_INCLUDED
#define PERF_SSL_SERVER_H_INCLUDED

#include<string>
#include<atomic>
#define OPENSSL
#include "TCPSSLServer.h"

namespace perf_monitor
{
	struct perf_ssl_server : public CTCPSSLServer
	{
		perf_ssl_server();
		~perf_ssl_server();

		const std::string& port() const { return m_TCPServer.port() ; }

		void start_listening();
		void initiate_shutdown();
		static int recv_req(SSLSocket connect_socket, char* data_buff, std::size_t sz);
		static void listener_thread(perf_ssl_server* server);
		static void client_thread(SSLSocket connected_socket, perf_ssl_server* server);
		static void trim_thread(perf_ssl_server* server);

		void SetSSLCertFile(const std::string& strPath) { CTCPSSLServer::SetSSLCertFile(strPath); }
		void SetSSLKeyFile(const std::string& strPath) { CTCPSSLServer::SetSSLKeyFile(strPath); }

		static std::atomic<bool> stopping;
		static std::atomic<int> num_connections;
	};
}


#endif // !PERF_SSL_SERVER
