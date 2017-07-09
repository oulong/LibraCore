#ifndef LIBRACPP_STREAM_H
#define LIBRACPP_STREAM_H

#include "libra.h"
#include "handle.h"

namespace Libra {

	template <typename T>
	class Stream : public Handle<T>
	{
        typedef Handle<T> base;
	public:
		//const UVError&
		bool listen(std::function<void(const UVError&)> callback, int backlog = 128)
		{
			callbacks::store<decltype(callback)>(this->get()->data, internal::uv_cid_listen, callback);

			return uv_listen(stream_ptr(), backlog, [](uv_stream_t* s, int status){
				
				callbacks::invoke<std::function<void(const UVError&)>>(
					s->data, internal::uv_cid_listen, UVError(status));
			}) == 0;

            return true;
		}
		
		bool accept(Stream* client)
		{
			return uv_accept(stream_ptr(), client->stream_ptr()) == 0;
		}

		bool read_start(std::function<void(const char* buffer, ssize_t len)> callback)
		{
			return read_start<0>(callback);
		}

		template<size_t max_alloc_size>
		bool read_start(std::function<void(const char* buffer, ssize_t len)> callback)
		{			
			callbacks::store<decltype(callback)>(this->get()->data, internal::uv_cid_read_start, callback);

			return uv_read_start(stream_ptr(),

				[](uv_handle_t*, size_t suggested_size, uv_buf_t* buf){

					int size = suggested_size > max_alloc_size ? suggested_size : max_alloc_size;

					buf->base = new char[size];
					buf->len = size;
				},

				[](uv_stream_t* s, ssize_t nread, const uv_buf_t* buf) {
					try
					{
						if (nread < 0)
						{
							callbacks::invoke<std::function<void(const char* buf, ssize_t len)>>(
								s->data, internal::uv_cid_read_start, nullptr, nread);
						}
						else if (nread >= 0)
						{
							callbacks::invoke<std::function<void(const char* buf, ssize_t len)>>(
								s->data, internal::uv_cid_read_start, buf->base, nread);
						}
						delete buf->base;
					}
					catch (const Exception& e)
					{
						delete buf->base;
						throw e;
					}
			}) == 0;
		}

		
		bool read_stop()
		{
			return uv_read_stop(stream_ptr()) == 0;
		}

		bool write(const char* data, size_t len, std::function<void(const UVError&)> callback)
		{		
#ifdef  __WIN32
			uv_buf_t bufs[] = { uv_buf_t{ static_cast<size_t>(len), const_cast<char*>(data) } };
#else
			uv_buf_t bufs[] = { uv_buf_t{ const_cast<char*>(data),static_cast<size_t>(len) } };
#endif
			callbacks::store<decltype(callback)>(this->get()->data, internal::uv_cid_write, callback);
			return uv_write(new uv_write_t, stream_ptr(), bufs, 1, [](uv_write_t* req, int status){
				try
				{
					callbacks::invoke<std::function<void(const UVError&)>>(
						req->handle->data, internal::uv_cid_write, UVError(status));
					delete req;
				}
				catch (const Exception& e)
				{
					delete req;
					throw e;
				}

			}) == 0;
		}

		bool shutdown(std::function<void(const UVError&)> callback)
		{
			callbacks::store<decltype(callback)>(this->get()->data, internal::uv_cid_shutdown, callback);
			return uv_shutdown(new uv_shutdown_t, stream_ptr(), [](uv_shutdown_t* req, int status) {
				try
				{
					callbacks::invoke<std::function<void(const UVError&)>>(
						req->handle->data, internal::uv_cid_shutdown, UVError(status));
					delete req;
				}
				catch (const Exception& e)
				{
					delete req;
					throw e;
				}

			}) == 0;
		}

		bool set_blocking(int blocking)
        {
			return uv_stream_set_blocking(stream_ptr(), blocking) == 0;
        }

	private:
		uv_stream_t* stream_ptr()
		{
			return 	Handle<T>::template get<uv_stream_t>();
		}
				
	};
}

#endif
