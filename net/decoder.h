#ifndef __LIBRA_DECODER_H__
#define __LIBRA_DECODER_H__

#include <functional>

#include "libra.h"
#include "buffer.h"
#include "tcp_server.h"
#include "packet.h"

namespace Libra
{
	namespace tcp_server
	{
		class Decoder : public ResolverImpl
		{
		public:						
			int handle(IBuffer* bufufer, std::function<void(IPacket*)> cb) override;
		};
	}
	
}

#endif