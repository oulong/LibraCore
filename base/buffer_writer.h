#ifndef __LIBRA_BUFFER_WRITER_H__
#define __LIBRA_BUFFER_WRITER_H__

#include "buffer.h"
#include "bytes.h"

namespace Libra
{
	class buffer_writer_stop : public Libra::Exception
	{
	public:
		buffer_writer_stop()
			: Libra::Exception("Buffer Writer", "Not enough space to write!")
		{

		}
	};
	///////////////////////////////////////////////////////////////////////////
	class BufferWriter
	{
	public:
		BufferWriter(IBuffer* buf)
			: buf_(buf)			
		{
			assert(buf);
		}
		
		template<class T, class _Swap = SwapH2N<T>>
		bool append(T v)
		{			
			v = _Swap()(v);
			return buf_->append((char*)&v, sizeof(T));			
		}

		template<class T, class _Swap = SwapH2N<T>>
		bool set(T v, size_t index)
		{
			v = _Swap()(v);
			return buf_->set((char*)&v, sizeof(T), index);
		}
		
		bool append_string(const std::string& s)
		{
			uint32_t size = s.size();
			if (!append<uint32_t>(size))
			{
				return false;
			}

			if (!buf_->append(s.c_str(), size))
			{
				buf_->remove(buf_->size() - sizeof(uint32_t), sizeof(uint32_t));
				return false;
			}

			return true;
		}

		bool append_bytes(const char* buf, size_t len)
		{
			return buf_->append(buf, len);
		}

	private:	
		IBuffer* buf_;		
	};
}

#endif