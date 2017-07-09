#ifndef __LIBRA_BUFFER_H__
#define __LIBRA_BUFFER_H__

#include "libra.h"
#include <vector>

namespace Libra
{	
	class IBuffer
	{
	public:
		IBuffer() = default;

		virtual ~IBuffer() = default;
		/// data size
		virtual size_t size() const = 0;
		//set size
		virtual void set_size(size_t s) = 0;
		/// buffer capacity
		virtual size_t capacity() const = 0;
		/// free capacity
		virtual size_t free_size() const = 0;	
		/// const byte by index
		virtual char operator[](int i) const = 0;
		/// reference byte by index
		virtual char& operator[](int i) = 0;
		////
		virtual bool set(char* buf, size_t len, size_t index = 0) const = 0;
		virtual bool get(char* buf, size_t len, size_t index = 0) const = 0;
		/// append data to buffer 
		virtual bool   append(const char *buf, size_t len) = 0;
		virtual size_t remove(size_t start, size_t len) = 0;
		/// fully copy
		virtual bool copy(const char *buf, size_t len) = 0;
		/// native buffer
		virtual char* data() = 0;
		/// native buffer
		virtual const char* data() const = 0;
		

		IBuffer &operator=(const IBuffer &buffer)
		{
			if (&buffer == this) return *this; 

			copy(buffer.data(), buffer.size());
			return *this;
		}
	protected:
		IBuffer(const IBuffer &buffer) = delete;	
	};
	
	class Buffer : public IBuffer
	{
	public:
		Buffer(char* buffer) 
			: data_(buffer)
			, ptr_(0)
		{			
			assert(buffer);
		}

		virtual ~Buffer()
		{			
			ptr_ = 0;
		}

		char* data() override
		{
			return data_;
		}

		const char* data() const override
		{
			return data_;
		}

		size_t size() const override
		{
			return ptr_;
		}

		void set_size(size_t size)
		{
			if (size > capacity())
			{
				throw Exception(__FUNCTION__
					, string_util::format("Over capacity.capacity:%d,size:%d", capacity(), size));
			}

			ptr_ = size;
		}	

		size_t free_size() const override
		{
			return capacity() - ptr_;
		}

		char operator[](int i) const override
		{
			if (i < 0 || (size_t)(i) >= size())
			{
				throw Exception(__FUNCTION__
					, string_util::format("Out valid index.index:%d,size:%d", i, size()));
			}

			return data_[i];
		}

		char& operator[](int i) override
		{
			if (i < 0 || (size_t)(i) >= size())
			{
				throw Exception(__FUNCTION__
					, string_util::format("Out valid index.index:%d,size:%d", i, size()));
			}

			return data_[i];
		}			
		
		bool get(char* buf, size_t len, size_t index = 0) const override
		{
			if (index + len > ptr_)
			{
				return false;
			}

			memcpy(buf, data_ + index, len);
			return true;
		}

		bool set(char* buf, size_t len, size_t index = 0) const override
		{
			if (index + len > ptr_)
			{
				return false;
			}

			memcpy(data_ + index, buf, len);
			return true;
		}

		bool append(const char* buf, size_t len) override
		{
			if (free_size() < len)
			{
				throw Exception(__FUNCTION__
					, string_util::format("Not enough free space! Free:%d,need:%d", free_size(), len));
			}

			memcpy(data_ + ptr_, buf, len);
			ptr_ += len;

			return true;
		}

		size_t remove(size_t start, size_t len) override
		{
			if (start >= ptr_) return 0;
			
			if (start + len >= ptr_)
			{	
				size_t m_size = ptr_ - start;
				ptr_ = start;
				return m_size;
			}
			else
			{
				memmove(data_ + start, data_ + start + len, ptr_ - len);
				ptr_ -= len;
				return len;
			}
		}

		virtual bool copy(const char *buf, size_t len) override
		{
			if (len > capacity())
			{				
				throw Exception(__FUNCTION__
					, string_util::format("Not enough capacity! Capacity:%d,need:%d", capacity(), len));
				
			}

			memcpy(data_, buf, len);
			ptr_ = len;

			return true;
		}
	protected:
		char*	data_;
		size_t  ptr_;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	class DynamicBuffer : public Buffer
	{
	public:		
		DynamicBuffer(uint32_t capacity) 
			: Buffer(new char[capacity])
			, capacity_(capacity)
		{			
			memset(data_, 0, capacity);
		}

		~DynamicBuffer()
		{
			delete data_;			
		}		

		size_t capacity() const override
		{
			return capacity_;
		}		
	private:
		uint32_t capacity_;	
	};

	template<ssize_t Capacity>
	class StaticBuffer : public Buffer
	{
	public:
		StaticBuffer() : Buffer(buf_)
		{
			memset(buf_, 0, Capacity);
		}
		
		size_t capacity() const override
		{
			return Capacity;
		}
	private:
		char buf_[Capacity];
	};

	class WrapBuffer : public Buffer
	{
	public:
		WrapBuffer(char* buff, size_t size)
			: Buffer(buff)
			, capacity_(size)
		{
			Buffer::set_size(size);
		}

		WrapBuffer(char* buff, size_t size, size_t capacity)
			: Buffer(buff)
			, capacity_(capacity)
		{		
			Buffer::set_size(size);
		}

		size_t capacity() const override
		{
			return capacity_;
		}
	private:
		ssize_t capacity_;
	};
}

#endif