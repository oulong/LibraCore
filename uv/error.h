#ifndef __LIBRA_ERROR_H__
#define __LIBRA_ERROR_H__

#include "uv.h"
#include "exception.h"

namespace Libra
{
	class UVError
	{
	public:
		UVError(int err) : uv_err_(err) {}
		~UVError() = default;

	public:
		operator bool() { return uv_err_ != 0; }
		operator bool() const { return uv_err_ != 0; }

		int code() const { return uv_err_; }
		const char* name() const { return uv_err_name(uv_err_); }
		const char* str() const { return uv_strerror(uv_err_); }

	private:
		int uv_err_{0};
	};

	class UVLibException : public Exception
	{
	public:
		UVLibException(const std::string& method, int error)
			: Exception(method, "", error)
		{
			char buf[512] = { 0 };
#ifdef __WIN32
			_sprintf_p(buf, 512 - 1, "(name:%s)=>%s!", uv_err_name(error), uv_strerror(error));
#else
			snprintf(buf, 512 - 1, "(name:%s)=>%s!", uv_err_name(error), uv_strerror(error));
#endif		
			message_ = buf;
		}
	};
}

#endif