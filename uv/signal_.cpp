#include "signal_.h"
#include <algorithm>

namespace Libra
{
	Signal::Signal()
	{	
		int r = uv_signal_init(uv_default_loop(), get<uv_signal_t>());
		if (r != 0) {
			throw UVLibException(__FUNCTION__, r);
		}		
	}

	bool Signal::bind(int signal_num, std::function<void(int)> callback)
	{		
		if (signal_num < 1 || signal_num > MAX_SIGNS)
		{
			throw Exception(__FUNCTION__, "params error");
		}

		callbacks::store(get()->data, signal_num, callback);
		return uv_signal_start(get<uv_signal_t>(), [](uv_signal_t* handle, int signum){			
			callbacks::invoke<std::function<void(int)>>(handle->data, signum, signum);
			
		}, signal_num) == 0;		
	}

	bool Signal::cancel()
	{
		return uv_signal_stop(get<uv_signal_t>()) == 0;		
	}	
#if 0
	SignalHandler::SignalHandler()
	{
	}

	SignalHandler::~SignalHandler()
	{		
		std::for_each(signals_handles_.begin(), signals_handles_.end(), 
			[](const std::pair<int, std::shared_ptr<Signal<SignalHandler>>>& it){
			if (uv_is_active(it.second->handle()))
			{
				it.second->cancel();
			}			
		});

		signals_cbs_.clear();
		signals_handles_.clear();
	}	

	void SignalHandler::bind_signal(int signal_num, const SignalFunc& func, uv_loop_t* loop)
	{
		assert(loop);

		std::map<int, SignalFunc>::iterator it = signals_cbs_.find(signal_num);
		if (it != signals_cbs_.end()) {
			throw Exception(__FUNCTION__, string_util::format("already bind the signal: %d", signal_num));
		}
		
		signals_cbs_[signal_num] = func;		
		try
		{			
			auto new_signal = std::make_shared<Signal<SignalHandler>>(this, loop);			
			new_signal->bind(signal_num);
			signals_handles_[signal_num] = new_signal;
		}
		catch (const Exception& e)
		{		
			signals_cbs_.erase(signal_num);
			throw e;
		}			
	}

	void SignalHandler::cancel_signal(int signal_num)
	{			
		std::map<int, SignalFunc>::iterator it = signals_cbs_.find(signal_num);
		if (it != signals_cbs_.end()) {			
			signals_cbs_.erase(it);
		}

		std::map<int, std::shared_ptr<Signal<SignalHandler>>>::iterator it_h
			= signals_handles_.find(signal_num);
		if (it_h != signals_handles_.end()) {
			it_h->second->cancel();			
		}

		//Fix: Function by bind_signal to call cancel_signal
		//signals_handles_.erase(it_h);
	}

	void SignalHandler::handle_signal(int signal_num)
	{
		std::map<int, SignalFunc>::iterator it = signals_cbs_.find(signal_num);
		if (it == signals_cbs_.end()) {
			throw Exception(__FUNCTION__, string_util::format("un bind the signal: %d", signal_num));
		}

		it->second(signal_num);
	}
#endif
}