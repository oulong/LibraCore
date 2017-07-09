#include "packet.h"
#include "buffer_reader.h"
#include "buffer_writer.h"

namespace Libra
{
	Packet::Packet()
	{
		
	}

	Packet::~Packet()
	{
	}

	void Packet::copy(const char* buffer, ssize_t len)
	{
		buffer_.copy(buffer, len);
	}

	uint16_t Packet::body_size() const
	{
		BufferReader reader(&buffer_, BODY_LEN_PTR);
		return reader.get<uint16_t>();
	}

	bool Packet::set_body_size()
	{
		BufferWriter writer(&buffer_);
		return writer.set<uint16_t>(buffer_.size() - Packet::HEADER_SIZE, BODY_LEN_PTR);
	}


	uint16_t Packet::cmd() const
	{
		BufferReader reader(&buffer_, CMD_PTR);
		return reader.get<uint16_t>();
	}

	bool Packet::set_cmd(uint16_t cmd)
	{
		BufferWriter writer(&buffer_);
		return writer.set<uint16_t>(cmd, CMD_PTR);
	}

	
	//int Packet::decode(IBuffer* buffer)
	//{	
	//	if (buffer->size() < Packet::HEADER_SIZE)
	//	{
	//		return -1;
	//	}

	//	BufferReader reader(buffer);
	//	char magic_a = reader.get<int8_t>();
	//	char magic_b = reader.get<int8_t>();
	//	if (magic_a != 'L' || magic_b != 'I')
	//	{
	//		throw Exception(__FUNCTION__, "magic code error!");
	//	}
	//	//
	//	body_len_ = reader.get<uint16_t>();
	//	cmd_ = reader.get<uint16_t>();

	//	debug_log("body_len: %u, Cmd: 0x%x", body_len_, cmd_);

	//	if (buffer->size() < (uint32_t)(Packet::HEADER_SIZE + body_len_))
	//	{
	//		return -1;
	//	}

	//	body_ = new char[body_len_];		
	//	reader.get_bytes(body_, body_len_);

	//	return Packet::HEADER_SIZE + body_len_;
	//}

	//int Packet::encode(IBuffer* buffer)
	//{
	//	//BufferWriter 
	//}

	////////////////////////////////////////////////////////////////////
	/*ReaderPacket::ReaderPacket(uint16_t cmd)
		: cmd_(cmd)
		, buffer_(0x2000)
	{
		buffer_.set_size(Packet::HEADER_SIZE);
	}

	void ReaderPacket::set_body(const char* buf, uint32_t len)
	{
		
	}*/
	/////////////////////////////////////////////////////////////////////
	WriterPacket::WriterPacket(uint16_t cmd)
	{
		buffer_.set_size(Packet::HEADER_SIZE);

		BufferWriter writer(&buffer_);

		writer.set<int8_t>('L', 0);
		writer.set<int8_t>('I', 1);

		Packet::set_cmd(cmd);
	}
}