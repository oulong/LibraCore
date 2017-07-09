#ifndef __LIBRA_ADDRESS_H__
#define __LIBRA_ADDRESS_H__

#include "libra.h"

namespace Libra
{
	class AddressCore;
	class Address
	{
	public:
		enum Family
		{
			V4,
			V6
		};
	public:
		//
		Address(const std::string& host, int port);
		Address(const struct sockaddr* addr, int length);

		std::string host() const;
		uint16_t port() const;
		int family() const;
		socklen_t length() const;
		const struct sockaddr* addr() const;
	private:
		std::shared_ptr < AddressCore > core_;
	};
}
#endif