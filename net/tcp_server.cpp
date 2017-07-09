#include "tcp_server.h"
#include "exception.h"

#include <iostream>
#include <functional>


namespace Libra {

	namespace tcp_server
	{
		Client::Client(Tcp * server, ResolverImpl* parse)
			: resolver_(std::shared_ptr<ResolverImpl>(parse))
			, socket_(nullptr)
			, recv_buffer_(64*1024)
			, packet_processor_(nullptr)
			, over_cb_(nullptr)
		{	
			socket_ = std::make_shared<Tcp>();			
			if (!server->accept(socket_.get()))
			{
				throw Exception(__FUNCTION__, "call accept failed!");
			}

			try
			{
				Address addr = socket_->peername();
				debug_log("new client host:%s,port:%d", addr.host().c_str(), addr.port());

				socket_->read_start([=](const char* buf, ssize_t len){
					if (buf == 0x00 && len < 0) {						
						//Address addr = socket_->peername();
						debug_log("client[%s:%d] disconnect.", addr.host().c_str(), addr.port());
						socket_->read_stop();
						except_error();
					}
					else {
						try
						{
							recv_buffer_.append(buf, len);
							resolver_->handle(&recv_buffer_
								, std::bind(&Client::process, this, std::placeholders::_1));
						}
						catch (const Exception& e)
						{
							//todo						
							warn_log("[%s] error:%s", __FUNCTION__, e.what());
							except_error();
						}
					}
				});
			}
			catch (const Libra::Exception& e)
			{
				error_log("[%s], error: %s", __FUNCTION__, e.what());
				except_error();
			}
		}

		Client::~Client()
		{
			debug_log("release client");
		}

		bool Client::normal() const
		{
			return socket_ && socket_->is_active();
		}

		void Client::send(const char* buf, ssize_t len)
		{		
			if (!socket_->is_active()) return;

			if (!socket_->write(buf, len, [=](const UVError& err){
				if (err)
				{
					error_log("[%s], error: %s", __FUNCTION__, err.str());
					except_error();
				}
				else
				{
					debug_log("send data[%d] success!", len);
				}

			})) {
				error_log("[%s], error: %s", __FUNCTION__, "send data error");
				except_error();
			}
		}

		void Client::set_hanlde_packet_cb(PACKET_CB packet_cb)
		{
			packet_processor_ = packet_cb;
		}

		void Client::set_over_cb(OVER_CB cb)
		{
			over_cb_ = cb;
		}

		void Client::over_read_data(size_t size)
		{
			recv_buffer_.remove(0, size);
		}

		void Client::except_error()
		{
			socket_->close();
			
			if (over_cb_)
			{
				try
				{
					over_cb_(this);
				}
				catch (const Libra::Exception& e)
				{
					error_log("[%s]: err: %s", __FUNCTION__, e.what());				
				}
			}
			else
			{
				warn_log("[%s:%d]Not setup install over client processor!", __FILE__, __LINE__);
			}

			delete this;
		}
		
		void Client::process(IPacket* packet)
		{
			if (packet_processor_)
			{
				try
				{
					packet_processor_(this, packet);					
				}
				catch (const Libra::Exception& e)
				{
					error_log("[%s]: err: %s", __FUNCTION__, e.what());
					//todo handle error;
				}
			}
			else
			{
				warn_log("[%s:%d]Not setup install packet processor!", __FILE__, __LINE__);
			}
		}
	}

	
	///////////////////////////////////////////////////////////////////////////////////////////////
	TCPServer::TCPServer()
		: sock_(std::make_shared<Tcp>())
	{	
			
	}

	TCPServer::~TCPServer()
	{
		if (sock_)
		{
			sock_->close();
		}
	}
}