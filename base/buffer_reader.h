#ifndef __LIBRA_BUFFER_READER_H__
#define __LIBRA_BUFFER_READER_H__

#include "buffer.h"
#include "bytes.h"

namespace Libra
{
	class buffer_reader_stop : public Libra::Exception
	{
	public:
		buffer_reader_stop()
			: Libra::Exception("Buffer Reader", "Not enough data to read!")
		{

		}
	};

	///////////////////////////////////////////////////////////////////////////
	class BufferReader
	{		
	public:
		BufferReader(const IBuffer* buf)
			: buf_(buf)
			, index_(0)
		{				
			assert(buf);
		}

		BufferReader(const IBuffer* buf, int index)
			: buf_(buf)
			, index_(index)
		{
			assert(buf);
		}

		template<class T, class _Swap = SwapN2H<T>>
		T get()
		{
			T v_{ 0 };
			if (! buf_->get((char*)&v_, sizeof(T), index_))
			{
				throw buffer_reader_stop();
			}

			index_ += sizeof(T);
			return _Swap()(v_);			
		}
		
		std::string get_string()
		{			
			uint32_t size = get<uint32_t>();
			std::string s;
			s.resize(size);
			
			if (! buf_->get(const_cast<char*>(s.c_str()), size, index_))
			{
				index_ -= sizeof(uint32_t);
				throw buffer_reader_stop();
			}			

			index_ += size;
			return std::move(s);
		}

		void get_bytes(char* buf, size_t len)
		{
			if (! buf_->get(buf, len, index_))
			{
				throw buffer_reader_stop();
			}

			index_ += len;
		}
	private:
		const IBuffer* buf_;
		int index_;
	};
}

#endif