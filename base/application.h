//
// Created by oulong on 17/6/2.
//

#ifndef LIBRACPP_APPLICATION_H
#define LIBRACPP_APPLICATION_H

#include "libra.h"
#include "non_copyable.h"
#include "signal_.h"

#include <functional>
#include <map>
#include <vector>

#include "tcp_server.h"

namespace Libra {

	
	class Application 			
	{		
	public:
		static Application* instance()
		{
			return Singleton < Application >::instance();
		}

		void run();

		void stop();

	public:
		Application();
		virtual ~Application() throw();		
	protected:
		uv_loop_t* loop_{uv_default_loop()};
		bool stop_{ false };
	};
}

#endif
