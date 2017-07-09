#include "address.h"

namespace Libra
{
	class AddressCore
	{
	public:			
		virtual std::string host() const = 0;
		virtual uint16_t port() const = 0;
		virtual int family() const = 0;
		virtual socklen_t length() const = 0;
		virtual const struct sockaddr* addr() const = 0;
	protected:
		AddressCore() {}
		virtual ~AddressCore() {}

	private:
		AddressCore(const AddressCore&) = delete;
		AddressCore& operator=(const AddressCore&) = delete;
	};

	class IPv4 : public AddressCore
	{
	public:
		IPv4(const void* addr, uint16_t port)
		{
			memset(&addr_, 0, sizeof(addr_));
			addr_.sin_family = AF_INET;
			memcpy(&addr_.sin_addr, addr, sizeof(addr_.sin_addr));
			addr_.sin_port = port;
		}

		IPv4(const struct sockaddr_in* addr)
		{
			addr_.sin_family = AF_INET;
			memcpy(&addr_, addr, sizeof(addr_));
		}

		std::string host() const override
		{
			char dest[16] = { 0 };
			if (uv_ip4_name(const_cast<sockaddr_in*>(&addr_), dest, 16) != 0)
				throw Exception(__FUNCTION__, "Cannot parse IPv4 hostname");
			return dest;
		}

		uint16_t port() const override
		{
			return addr_.sin_port;
		}

		int family() const override
		{
			return addr_.sin_family;
		}

		socklen_t length() const override
		{
			return sizeof(addr_);
		}

		const struct sockaddr* addr() const override
		{
			return reinterpret_cast<const struct sockaddr*>(&addr_);
		}
	private:
		struct sockaddr_in addr_;
	};
	//--------------------------------------------------------------------------------------

	Address::Address(const std::string& host, int port)
	{			
		char ia[sizeof(struct in6_addr)];
		if (uv_inet_pton(AF_INET, host.c_str(), &ia) == 0)
		{			
			core_ = std::make_shared<IPv4>((void*)&ia, htons(port));	
		}			
		else if (uv_inet_pton(AF_INET6, host.c_str(), &ia) == 0)
		{
			throw NotImplementInterface(__FUNCTION__);
		}
		else
			throw Exception(__FUNCTION__, "Invalid IP address format: " + host);

	}

	Address::Address(const struct sockaddr* addr, int length)
	{
		if (length == sizeof(struct sockaddr_in))
			core_ = std::make_shared<IPv4>(
			reinterpret_cast<const struct sockaddr_in*>(addr));
		else if (length == sizeof(struct sockaddr_in6))
			throw NotImplementInterface(__FUNCTION__);
		else
			throw Exception(__FUNCTION__, "Invalid address length passed to Address()");
	}

	std::string Address::host() const
	{
		return core_->host();
	}

	uint16_t Address::port() const
	{
		return core_->port();
	}

	int Address::family() const
	{
		return core_->family();
	}

	socklen_t Address::length() const
	{
		return core_->length();
	}

	const struct sockaddr* Address::addr() const
	{
		return core_->addr();
	}
}

