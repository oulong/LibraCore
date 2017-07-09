#ifndef LIBRACPP_SINGLETON_H
#define LIBRACPP_SINGLETON_H

#include <thread>         // std::thread
#include <mutex>          // std::timed_mutex

namespace Libra
{
	template <class T>
	struct CreateUsingNew
	{
		static T* create()
		{
			return new T;
		}

		static void destroy(T* p)
		{
			if (p) {
				delete p;			
			}
		}
	};
	//----------------------------------------------------------------------

	template <class T, template<class> class CreationPolicy = CreateUsingNew>
	class Singleton
	{
	public:
		static T* instance()
		{
			if (nullptr == ptr_)
			{
				//guard
				std::lock_guard<std::mutex> guard(m_);
				if (nullptr == ptr_)
				{
					ptr_ = CreationPolicy<T>::create();
				}
			}

			return ptr_;
		}

		static void destory()
		{
			if (ptr_)
			{
				//guard
				std::lock_guard<std::mutex> guard(m_);
				if (ptr_)
				{
					CreationPolicy<T>::destroy(ptr_);					
				}
			}
		}

	private:
		Singleton();
		Singleton(const Singleton&);
		Singleton& operator=(const Singleton&);
	private:
		static std::mutex m_;
		static T* ptr_;		
	};

	template <class T, template<class> class CreationPolicy>
	std::mutex Singleton<T, CreationPolicy>::m_;

	template <class T, template<class> class CreationPolicy>
	T* Singleton<T, CreationPolicy>::ptr_ = nullptr;
}

#endif