//
// Created by dawx62fac on 17/6/1.
//

#ifndef LIBRACPP_TCPSERVER_H
#define LIBRACPP_TCPSERVER_H

#include <string>

#include "libra.h"
#include "tcp.h"
#include "buffer.h"
#include "packet_impl.h"


namespace Libra {
	
	namespace tcp_server
	{
		class ResolverImpl;
		class Client;

		typedef std::function<int(Client* c, IPacket* p)> PACKET_CB;
		typedef std::function<void(Client* c)> OVER_CB;
		
		class Client
		{
		public:
			Client(Tcp * server, ResolverImpl* parse);
			~Client();

			IBuffer* recv_buffer() { return &recv_buffer_; }
			void over_read_data(size_t size);

			void set_hanlde_packet_cb(PACKET_CB packet_cb);
			void set_over_cb(OVER_CB over_cb);

			void process(IPacket* packet);

			bool normal() const;

			void send(const char* buf, ssize_t len);
		private:
			void except_error();
		private:
			std::shared_ptr<ResolverImpl> resolver_;
			std::shared_ptr<Tcp> socket_;
			
			DynamicBuffer recv_buffer_;

			PACKET_CB packet_processor_;
			OVER_CB	  over_cb_;
		};
		

		class ResolverImpl
		{
		public:
			virtual ~ResolverImpl() = default;
			virtual int handle(IBuffer* bufufer, std::function<void(IPacket*)> cb) = 0;			
		};
	}

	class TCPServer
	{
	public:
		TCPServer();
		~TCPServer();

		template<typename T_Resolver>
		bool start(const std::string& ip, int port, tcp_server::PACKET_CB packet_cb, tcp_server::OVER_CB over_cb)
		{
			if (!sock_->bind(ip, port)) return false;

			if (!sock_->listen([=](const UVError& error){

				if (error)
				{
					//TODO error handle
					std::cout << "listen err: " << error.str() << std::endl;
				}
				else
				{
					std::cout << "new connect ..............." << std::endl;
					tcp_server::Client* client = nullptr;
					try
					{						
						client = new tcp_server::Client(sock_.get(), new T_Resolver);
						client->set_hanlde_packet_cb(packet_cb);
						client->set_over_cb(over_cb);
					}
					catch (const Libra::Exception& e)
					{
						(void)e;
						//todo failed
						warn_log("error: %s", e.what());
						if (client) delete client;					
					}
					catch (...)
					{
						error_log("unknow exception!");
						//todo
						if (client) delete client;
					}
				}
			})) return false;

			return true;
		}
	private:
		std::shared_ptr<Tcp> sock_;
	};
}

#endif