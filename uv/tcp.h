#ifndef __LIBRA_TCP_H__
#define __LIBRA_TCP_H__

#include "libra.h"
#include "stream.h"
#include "address.h"

namespace Libra
{
	class Tcp : public Stream < uv_tcp_t >
	{
	public:
		Tcp();

		bool nodelay(bool enable);
		bool keepalive(bool enable, unsigned int delay);
		bool simultanious_accepts(bool enable);

		bool connect(const std::string& ip, int port, std::function<void(const UVError&)> callback);
		bool bind(const std::string& ip, int port);
		
		Address peername();
		Address sockname();
	};
}

#endif