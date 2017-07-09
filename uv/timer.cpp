#include "timer.h"

namespace Libra
{
	Timer::Timer()
	{			
		uv_timer_init(uv_default_loop(), get<uv_timer_t>());		
	}

	Timer::~Timer()
	{	
		//
	}

	int Timer::start(std::function<void()> callback, uint64_t timeout, uint64_t repeat)
	{			
		callbacks::store(get()->data, internal::uv_cid_timer_start, callback);
		return uv_timer_start(get<uv_timer_t>(), [](uv_timer_t* handle){			
			callbacks::invoke<std::function<void()>>(handle->data, internal::uv_cid_timer_start);
		}, timeout, repeat) == 0;
	}

	void Timer::stop()
	{		
		uv_timer_stop(get<uv_timer_t>());
	}
}