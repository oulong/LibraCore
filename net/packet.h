#ifndef __LIRBA_PACKET_H__
#define __LIRBA_PACKET_H__

#include "libra.h"
#include "buffer.h"
#include "packet_impl.h"
#include "buffer_reader.h"
#include "buffer_writer.h"

namespace Libra
{
	class Packet : public IPacket
	{
	public:
		enum {
			/*|LI|body_len|cmd|*/
			/*|2 |   2    | 2 |*/
			HEADER_SIZE = 6,
		};

		enum {
			MAGIC_PTR	 = 0,
			BODY_LEN_PTR = MAGIC_PTR + sizeof(char) * 2,
			CMD_PTR = BODY_LEN_PTR + sizeof(uint16_t),
		};
	public:
		Packet();
		~Packet();
	public:	
		void copy(const char* buffer, ssize_t len);

		const char* data() const { return buffer_.data(); }
		size_t data_size() const { return buffer_.size(); }
		
		uint16_t	body_size() const;
		bool		set_body_size();

		uint16_t	cmd() const;
		bool		set_cmd(uint16_t cmd);		
	protected:
		StaticBuffer<0x10000> buffer_;		
	};

	class ReaderPacket : public Packet
	{
	public:
		ReaderPacket() 
			: reader_(&buffer_, Packet::HEADER_SIZE)
		{
		}
		~ReaderPacket() {}
	public:		
		template<class T> T read()
		{			
			return reader_.get<T>();
		}

		std::string read_string()
		{
			return reader_.get_string();
		}
	private:
		BufferReader reader_;
	};

	class WriterPacket : public Packet
	{
	public:
		WriterPacket(uint16_t cmd);

		template<class T> bool append(T v)
		{
			BufferWriter writer(&buffer_);
			return writer.append<T>(v);
		}

		bool append_string(const std::string& s)
		{
			BufferWriter writer(&buffer_);
			return writer.append_string(s);
		}
	
		const IBuffer*	buffer()  { return &buffer_; }
	};
};

#endif