#ifndef __LIBRA_SOCKET_H__
#define __LIBRA_SOCKET_H__

#include "libra.h"
#include <string>

namespace Libra
{
	/// Base socket implementation from which all sockets derive.
	class Socket
	{
	public:	
		Socket() = default;
		virtual ~Socket() = default;

		/// Connects to the given peer IP address.
		///
		/// Throws an exception if the address is malformed.
		/// Connection errors can be handled via the Error signal.
		//virtual void connect(const Address& address) = 0;

		/// Resolves and connects to the given host address.
		///
		/// Throws an Exception if the host is malformed.
		/// Since the DNS callback is asynchronous implementations need
		/// to listen for the Error signal for handling connection errors.
		virtual void connect(const std::string& host, uint16_t port) = 0;

		/// Bind a local address to the socket.
		/// The address may be IPv4 or IPv6 (if supported).
		///
		/// Throws an Exception on error.
		//virtual void bind(const Address& address, unsigned flags = 0) = 0;
		virtual void bind(const std::string& host, unsigned flags = 0) = 0;

		/// Listens the socket on the given address.
		///
		/// Throws an Exception on error.
		virtual void listen(int backlog = 64) { (void)backlog; };

		/// Sends the shutdown packet which should result is socket
		/// closure via callback.
		virtual bool shutdown()
		{
			assert(0 && "not implemented by protocol");
			return false;
		};

		/// Closes the underlying socket.
		virtual void close() = 0;

		/// The locally bound address.
		///
		/// This function will not throw.
		/// A Wildcard 0.0.0.0:0 address is returned if
		/// the socket is closed or invalid.
		//virtual Address address() const = 0;

		/// The connected peer address.
		///
		/// This function will not throw.
		/// A Wildcard 0.0.0.0:0 address is returned if
		/// the socket is closed or invalid.
		//virtual Address peerAddress() const = 0;

		/// The transport protocol: TCP, UDP or SSLTCP.
		//virtual net::TransportType transport() const = 0;

		/// Sets the socket error.
		///
		/// Setting the error will result in socket closure.
		//virtual void setError(const scy::Error& err) = 0;

		/// Return the socket error if any.
		//virtual const scy::Error& error() const = 0;

		/// Returns true if the native socket handle is closed.
		virtual bool closed() const = 0;

		/// Returns the socket event loop.
		//virtual uv::Loop* loop() const = 0;

		/// Optional client data pointer.
		///
		/// The pointer is set to null on initialization
		/// but not managed.
		void* opaque{ nullptr };

	protected:
		/// Initializes the underlying socket context.
		//virtual void init() = 0;

		/// Resets the socket context for reuse.
		//virtual void reset() = 0;

		int _af{ AF_UNSPEC };
	};

}

#endif