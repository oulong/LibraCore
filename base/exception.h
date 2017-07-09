//
// Created by oulong on 17/5/31.
//

#ifndef LIBRACPP_EXCEPTION_H
#define LIBRACPP_EXCEPTION_H

#include <errno.h>
#include <string>
#include <exception>
#include <string.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "uv.h"

namespace Libra
{
    class Exception : public std::exception
    {
    public:
        Exception(const std::string& method, const std::string& message, int code = -1)
                : method_(method), message_(message), code_(code)
        {
			what_ = (method_ + "[method <--> message ] " + message_);
		}

        virtual ~Exception() {}

        const std::string& getMethod()  const throw() { return method_;  }
        const std::string& getMessage() const throw() { return message_; }
        int getCode()              const throw() { return code_;    }

        virtual const char* what() const throw() {
			return what_.c_str();
        }

    protected:
        std::string method_;    ///< Method, which throwed the eception
        std::string message_;   ///< Errormessage
        int code_;              ///< Errorcode

		std::string what_;
    };

    /**
     * 未实现的接口
     */
	class NotImplementInterface : public Exception
    {
    public:
		NotImplementInterface(const std::string& method) throw()
                : Exception(method, "Not implement the function")
        {}
    };
}



#endif //LIBRACPP_EXCEPTION_H
