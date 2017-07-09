#include "tcp.h"
#include "address.h"

namespace Libra
{
	Tcp::Tcp()
	{		
		int ret = uv_tcp_init(uv_default_loop(), get<uv_tcp_t>());
		if (ret)
		{
			throw UVLibException(__FUNCTION__, ret);
		}		
	}

	bool Tcp::nodelay(bool enable)
	{
		return uv_tcp_nodelay(get<uv_tcp_t>(), enable ? 1: 0) == 0;
	}

	bool Tcp::keepalive(bool enable, unsigned int delay)
	{ 
		return uv_tcp_keepalive(get<uv_tcp_t>(), enable ? 1 : 0, delay) == 0; 
	}

	bool Tcp::simultanious_accepts(bool enable)
	{
		return uv_tcp_simultaneous_accepts(get<uv_tcp_t>(), enable ? 1 : 0) == 0;
	}

	bool Tcp::connect(const std::string& ip, int port, std::function<void(const UVError&)> callback)
	{
		callbacks::store<decltype(callback)>(get()->data, internal::uv_cid_connect, callback);
		
		Address address(ip, port);		
		return uv_tcp_connect(new uv_connect_t, get<uv_tcp_t>()
			, address.addr()
			, [](uv_connect_t* req, int status){
			try
			{
				callbacks::invoke<std::function<void(const UVError&)>>(
					req->handle->data, internal::uv_cid_connect, UVError(status));
				delete req;
			}
			catch (const Exception& e)
			{
				delete req;
				throw e;
			}			
		}) == 0;
	}

	bool Tcp::bind(const std::string& ip, int port)
	{
		return uv_tcp_bind(get<uv_tcp_t>(), Address(ip, port).addr(), 0) == 0;
	}

	Address Tcp::peername()
	{
		struct sockaddr addr;
		int len = sizeof(sockaddr);
		int ret = uv_tcp_getpeername(get<uv_tcp_t>(), &addr, &len);
		if (ret == 0)
		{				
			return std::move(Address(&addr, len));
		}
		else
		{
			throw UVLibException(__FUNCTION__, ret);
		}		
	}

	Address Tcp::sockname()
	{
		struct sockaddr addr;
		int len = sizeof(sockaddr);
		int ret = uv_tcp_getsockname(get<uv_tcp_t>(), &addr, &len);
		if (ret == 0)
		{
			return std::move(Address(&addr, len));
		}
		else
		{
			throw UVLibException(__FUNCTION__, ret);
		}
	}
}