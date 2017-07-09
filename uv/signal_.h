#ifndef LIBRACPP_SIGNAL_H
#define LIBRACPP_SIGNAL_H

#include "libra.h"
#include "non_copyable.h"

#include <map>
#include <functional>

namespace Libra {

	enum {
		MAX_SIGNS = 64,
	};

	class Signal : public Handle<uv_signal_t, MAX_SIGNS + 1>
	{
		
	public:
		Signal();
		virtual ~Signal() = default;

		bool bind(int signal_num, std::function<void(int)> callback);
		bool cancel();
		
	};

	/*class SignalHandler : public NonCopyable
	{		
	public:
		typedef std::function<void(int signal_num)> SignalFunc;
		friend class Signal < SignalHandler >;

		SignalHandler();
		virtual ~SignalHandler();		
		
		void bind_signal(int signal_num, const SignalFunc& func, uv_loop_t* loop = uv_default_loop());
		void cancel_signal(int signal_num);
	protected:
		void handle_signal(int signal_num);
	private:
		std::map<int, SignalFunc> signals_cbs_;		
		std::map<int, std::shared_ptr<Signal<SignalHandler>>> signals_handles_;
	};*/
}

#endif