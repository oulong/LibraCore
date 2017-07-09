#ifndef LIBRACPP_HANDLE_H
#define LIBRACPP_HANDLE_H

#include "libra.h"
#include "callback.h"

namespace Libra
{
	namespace internal
	{
		enum uv_callback_id
		{
			uv_cid_close = 0,  //system 
			uv_cid_listen,
			uv_cid_read_start,
			uv_cid_write,
			uv_cid_shutdown,
			uv_cid_connect,
			uv_cid_timer_start,
			uv_cid_max
		};
	}	

	template<typename T, int ID_MAX = internal::uv_cid_max>
	class Handle
	{
	public:
		Handle()
		{
			uv_handle_ = new T;
			assert(uv_handle_);
			uv_handle_->data = new callbacks(ID_MAX);
			assert(uv_handle_->data);
		}

		virtual ~Handle()
		{
			//printf("~handle(): %x\n", this);
			uv_handle_ = nullptr;
		}

		template<typename U = uv_handle_t>
		U* get() { return reinterpret_cast<U*>(uv_handle_); }

		template<typename U = uv_handle_t>
		const U* get() const { return reinterpret_cast<U*>(uv_handle_); }

		bool is_active() { return uv_handle_ != nullptr && uv_is_active(get()) != 0; }
		bool is_closing() { return uv_handle_ == nullptr || uv_is_closing(get()) != 0; }

		void close(std::function<void()> callback = [](){})
		{			
			if (uv_is_closing(get()))
			{
				return; 
			}

			callbacks::store(get()->data, 0, callback);
			uv_close(get(), [](uv_handle_t* handle){
				callbacks::invoke<std::function<void()>>(handle->data, 0);

				if (handle->data)
				{
					delete reinterpret_cast<callbacks*>(handle->data);
					handle->data = nullptr;
				}

				delete reinterpret_cast<T*>(handle);
			});
		}
		
	private:
		/*Handle& operator =(const Handle& h)
		{
			uv_handle_ = h.uv_handle_;
			return *this;
		}*/
		Handle& operator = (const Handle&) = delete;
		Handle(const Handle&) = delete;
	protected:
		T* uv_handle_;
	};
}
	
#endif