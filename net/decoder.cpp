#include "decoder.h"
#include "packet.h"
#include "buffer_reader.h"

namespace Libra
{
	namespace tcp_server
	{			
		int Decoder::handle(IBuffer* buffer, std::function<void(IPacket*)> cb)
		{			
			int cnt = 0;
			do
			{
				if (buffer->size() < Packet::HEADER_SIZE)
				{
					break;
				}

				BufferReader reader(buffer);
				char magic_a = reader.get<int8_t>();
				char magic_b = reader.get<int8_t>();
				if (magic_a != 'L' || magic_b != 'I')
				{
					throw Exception(__FUNCTION__, "magic code error!");
				}
				//
				uint16_t body_len = reader.get<uint16_t>();
				uint16_t cmd = reader.get<uint16_t>();

				debug_log("body_len: %u, Cmd: 0x%x", body_len, cmd);

				uint32_t total_len = (uint32_t)(Packet::HEADER_SIZE + body_len);
				if (buffer->size() < total_len)
				{
					break ;
				}

				ReaderPacket packet;
				packet.copy(buffer->data(), total_len);
				////to  Decrypt					
				buffer->remove(0, total_len);				
				if (cb)
				{
					cb(&packet);
				}				
				
			} while (++cnt < 5);

			return 0;
		}
	}
}