#ifndef __LIBRA_CALLBACK_H__
#define __LIBRA_CALLBACK_H__

#include "libra.h"
#include "apply.h"

namespace Libra
{
	namespace internal
	{
		class callback_object_base
		{
		public:
			callback_object_base(void* data)
				: data_(data)
			{
			}
			virtual ~callback_object_base()
			{
			}

			void* get_data() { return data_; }

			void set_data(void* ptr) { data_ = ptr; }

		private:
			void* data_;
		};

		// SCO: serialized callback object
		template<typename callback_t>
		class callback_object : public callback_object_base
		{
		public:
			callback_object(const callback_t& callback, void* data = nullptr)
				: callback_object_base(data)
				, callback_(callback)
			{
			}

			virtual ~callback_object()
			{
			}

		public:
			template<typename ...A>
			typename std::result_of<callback_t(A...)>::type invoke(A&& ... args)
			{				
				return callback_(std::forward<A>(args)...);					
			}

			template<typename ...A>
			typename std::result_of<callback_t(A...)>::type 
				invoke_by_tuple(const std::tuple<A ...>& args)
			{			
				return apply<callback_t>(std::move(callback_), args);
			}

		private:
			callback_t callback_;
		};
	}

	typedef std::shared_ptr<internal::callback_object_base> callback_object_ptr;

	class callbacks
	{
	public:
		callbacks(int max_callbacks)
			: lut_(max_callbacks)
		{
		}
		~callbacks()
		{
		}

		template<typename callback_t>
		static void store(void* target, int cid, const callback_t& callback, void* data = nullptr)
		{			
			reinterpret_cast<callbacks*>(target)->lut_[cid] 
				= std::make_shared<internal::callback_object<callback_t>>(callback, data);
		}

		template<typename callback_t>
		static void* get_data(void* target, int cid)
		{
			return reinterpret_cast<callbacks*>(target)->lut_[cid]->get_data();
		}

		template<typename callback_t>
		static void set_data(void* target, int cid, void* data)
		{
			reinterpret_cast<callbacks*>(target)->lut_[cid]->set_data(data);
		}

		
		template<typename callback_t, typename ...A>
		static typename std::result_of<callback_t(A...)>::type invoke(void* target, int cid, A&& ... args)
		{
			
			auto x = dynamic_cast<internal::callback_object<callback_t>*>(
				reinterpret_cast<callbacks*>(target)->lut_[cid].get());
			assert(x);
			return x->invoke(std::forward<A>(args)...);
			
		}	

		template<typename callback_t, typename ...A>
		static  typename std::result_of<callback_t(A...)>::type 
			invoke_by_tuple(void* target, int cid, const std::tuple<A...>& params)
		{

			auto x = dynamic_cast<internal::callback_object<callback_t>*>(
				reinterpret_cast<callbacks*>(target)->lut_[cid].get());
			assert(x);			
			return x->invoke_by_tuple<A ...>(params);
		}
	private:
		std::vector<callback_object_ptr> lut_;
	};
}

#endif