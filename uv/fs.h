#ifndef __LIBRA_FS_H__
#define __LIBRA_FS_H__

#include "libra.h"
#include <string>

namespace Libra
{
	namespace fs
	{
		typedef uv_file file_handle;
	}
	namespace internal
	{
		fs::file_handle open(const std::string& path, int flags, int mode)
		{
			uv_fs_t req;
			int ret = uv_fs_open(uv_default_loop(), &req, path.c_str(), flags, mode,  nullptr);
			if (ret < 0)
			{
				throw Libra::UVLibException(__FUNCTION__, ret);
			}

			return ret;
		}

		bool open(const std::string& path, int flags, int mode, std::function<void(fs::file_handle fd, error e)> callback)
		{
			auto req = internal::create_req(callback);
			if (uv_fs_open(uv_default_loop(), req, path.c_str(), flags, mode, [](uv_fs_t* req) {
				assert(req->fs_type == UV_FS_OPEN);

				if (req->errorno) internal::invoke_from_req<decltype(callback)>(req, file_handle(-1), error(req->errorno));
				else internal::invoke_from_req<decltype(callback)>(req, req->result, error(req->result<0 ? UV_ENOENT : UV_OK));

				internal::delete_req(req);
			})) {
				// failed to initiate uv_fs_open()
				internal::delete_req(req);
				return false;
			}
			return true;
		}
	}
}

#endif