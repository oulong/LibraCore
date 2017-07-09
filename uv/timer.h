#ifndef LIBRACPP_TIMER_H
#define LIBRACPP_TIMER_H

#include "libra.h"

namespace Libra {
	
	class Timer : public Handle < uv_timer_t >
	{		
	public:
		Timer();
		~Timer();

		int start(std::function<void()> callback, uint64_t timeout, uint64_t repeat);
		void stop();
	};
}

#endif