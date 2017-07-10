#ifndef __LIBRA_POOL_H__
#define __LIBRA_POOL_H__

#include "libra.h"
#include "apply.h"

namespace Libra
{
	namespace pool
	{
		namespace internal
		{
			template<typename work_callback_t, typename over_callback_t>
			uv_work_t* create_req(work_callback_t work_cb, over_callback_t over_cb, void* data = nullptr)
			{
				auto req = new uv_work_t;
				req->data = new callbacks(2);
				assert(req->data);
				callbacks::store(req->data, 0, work_cb, data);
				callbacks::store(req->data, 1, over_cb, nullptr);

				return req;
			}

			void delete_req(uv_work_t* req, void* data = nullptr)
			{
				delete reinterpret_cast<callbacks*>(req->data);
				delete req;
			}	

			template<typename callback_t, typename data_t>
			data_t* get_data_params_from_req(uv_work_t* req)
			{
				return reinterpret_cast<data_t*>(callbacks::get_data<callback_t>(req->data, 0));
			}

			template<typename data_t>
			data_t* get_result_ptr_from_req(uv_work_t* req)
			{
				return (reinterpret_cast<data_t*>(callbacks::get_data<std::function<void(int, void*)>>(req->data, 1)));
			}	

			template<typename data_t>
			void set_result_ptr_from_req(uv_work_t* req, void* data)
			{
				callbacks::set_data<std::function<void(int, void*)>>(req->data, 1, data);
			}

			template<typename callback_t, typename ...A>
			typename std::result_of<callback_t(A...)>::type invoke_from_req(uv_work_t* req, A&& ... args)
			{
				return callbacks::invoke<callback_t>(req->data, 0, std::forward<A>(args)...);
			}

			template<typename callback_t, typename ...A>
			auto invoke_work(uv_work_t* req) throw()
				-> typename std::enable_if< !std::is_void<typename std::result_of<callback_t(A...)>::type>::value, void* >::type
			{
				typedef typename std::result_of<callback_t(A...)>::type type;
				type* r = nullptr;
				auto params = get_data_params_from_req<callback_t, std::tuple<A...>>(req);
				try
				{					
					r = new type;
					*r = std::move(callbacks::invoke_by_tuple<callback_t, A...>(req->data, 0, *params));	
					internal::set_result_ptr_from_req<type>(req, r);					
				}				
				catch (const std::exception& e)
				{
					if (r)
					{
						delete r;
						r = nullptr;
					}
					(void)(e);
				}
				catch (...)
				{
					if (r)
					{
						delete r;
						r = nullptr;
					}
				}

				if (params) delete params;
				
				return r;
			}
		
			template<typename callback_t, typename ...A>
			auto invoke_work(uv_work_t* req) throw()
				-> typename std::enable_if <std::is_void<typename std::result_of<callback_t(A...)>::type>::value, void>::type
			{
				auto params = get_data_params_from_req<callback_t, std::tuple<A...>>(req);
				try
				{
					callbacks::invoke_by_tuple<callback_t, A...>(req->data, 0, *params);
				}				
				catch (const std::exception& e)
				{
					(void)(e);
				}
				catch (...)
				{
				}

				if (params) delete params;
			}			

			template<typename callback_t, typename ...A>
			void invoke_over(uv_work_t* req, int error)
			{
				void* p = get_result_ptr_from_req<void>(req);
				try
				{				
					callbacks::invoke<std::function<void(int, void*)>>(
						req->data, 1, error, p);
					if (p)
					{
						typedef typename std::result_of<callback_t(A...)>::type type;
						delete reinterpret_cast<type*>(p);
					}
					
					delete_req(req);
				}
				catch (const Libra::Exception& e)
				{
					if (p)
					{
						typedef typename std::result_of<callback_t(A...)>::type type;
						delete reinterpret_cast<type*>(p);
						delete p;
					}
					delete p;
					delete_req(req);
					throw e;
				}				
			}
		}

		template<typename work_callback_t, typename ...A>
		bool run(work_callback_t work_cb, std::function<void(int, void*)> over_cb, A ... args)
		{

			auto params = new std::tuple<A...>(args ...);
			uv_work_t* req = internal::create_req(work_cb, over_cb, (void*)params);
			
			int ret = uv_queue_work(uv_default_loop(), req, [](uv_work_t* req){				
				
				try
				{
					internal::invoke_work<work_callback_t, A...>(req);					
				}
				catch (const Libra::Exception& e)
				{
					//todo notify
					error_log("[%s]--error:%s", __FUNCTION__, e.what());
				}

			}, [](uv_work_t* req, int err){	
				
				try
				{
					internal::invoke_over<work_callback_t, A...>(req, err);
				}
				catch (const Libra::Exception& e)
				{
					//todo notify
					error_log("[%s]--error:%s", __FUNCTION__, e.what());
				}

			});

			if (ret != 0)
			{
				throw Libra::UVLibException(__FUNCTION__, ret);
			}

			return ret == 0;
		}

	}
}

#endif