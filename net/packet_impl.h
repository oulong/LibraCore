#ifndef __LIBRA_IPACKET_H__
#define __LIBRA_IPACKET_H__

namespace Libra
{
	class IPacket
	{
	public:
		virtual ~IPacket() = default;

		//virtual IBuffer*    body() = 0;
		//virtual uint16_t	body_size() const = 0;
	};
}

#endif