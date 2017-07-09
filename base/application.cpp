#include "application.h"

namespace Libra {
	
	Application::Application()		
	{
		assert(this->loop_ != nullptr);
	}

	Application::~Application() throw()
	{
		if (!stop_)
		{
			uv_loop_close(loop_);
		}		
	}

	void Application::run()
	{
		uv_run(loop_, UV_RUN_DEFAULT);
	}	

	void Application::stop()
	{
		if (uv_loop_alive(loop_)) {
			uv_stop(loop_);
			stop_ = true;
		}
	}
}