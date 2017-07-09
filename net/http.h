#ifndef __LIBRA_HTTP_H__
#define __LIBRA_HTTP_H__

#include "libra.h"
#include "tcp.h"
#include "http_parser.h"

namespace Libra
{
	namespace http
	{
		class url_parse_exception : public Exception
		{
		public:
			url_parse_exception(const std::string& message = "Failed to parse URL.")
				: Exception("HTTP", message)
			{}
		};

		class response_exception : public Exception
		{
		public:
			response_exception(const std::string& message = "HTTP respsonse error.")
				: Exception("HTTP", message)
			{}
		};

		class client_context;
		typedef std::shared_ptr<client_context> http_client_ptr;
		class url_obj
		{
			friend class client_context;

		public:
			url_obj()
				: handle_(), buf_()
			{
			}

			url_obj(const url_obj& c)
				: handle_(c.handle_), buf_(c.buf_)
			{
			}

			url_obj& operator =(const url_obj& c)
			{
				handle_ = c.handle_;
				buf_ = c.buf_;
				return *this;
			}

			~url_obj()
			{
			}

		public:
			std::string schema() const
			{
				if (has_schema()) return buf_.substr(handle_.field_data[UF_SCHEMA].off, handle_.field_data[UF_SCHEMA].len);
				return "HTTP";
			}

			std::string host() const
			{
				// TODO: if not specified, use host name
				if (has_schema()) return buf_.substr(handle_.field_data[UF_HOST].off, handle_.field_data[UF_HOST].len);
				return std::string("localhost");
			}

			int port() const
			{
				if (has_path()) return static_cast<int>(handle_.port);
				return (schema() == "HTTP" ? 80 : 443);
			}

			std::string path() const
			{
				if (has_path()) return buf_.substr(handle_.field_data[UF_PATH].off, handle_.field_data[UF_PATH].len);
				return std::string("/");
			}

			std::string query() const
			{
				if (has_query()) return buf_.substr(handle_.field_data[UF_QUERY].off, handle_.field_data[UF_QUERY].len);
				return std::string();
			}

			std::string fragment() const
			{
				if (has_query()) return buf_.substr(handle_.field_data[UF_FRAGMENT].off, handle_.field_data[UF_FRAGMENT].len);
				return std::string();
			}

		private:
			void from_buf(const char* buf, std::size_t len, bool is_connect = false)
			{
				// TODO: validate input parameters

				buf_ = std::string(buf, len);
				if (http_parser_parse_url(buf, len, is_connect, &handle_) != 0)
				{
					// failed for some reason
					// TODO: let the caller know the error code (or error message)
					throw url_parse_exception();
				}
			}

			bool has_schema() const { return (handle_.field_set & (1 << UF_SCHEMA)) > 0; }
			bool has_host() const { return (handle_.field_set & (1 << UF_HOST)) > 0; }
			bool has_port() const { return (handle_.field_set & (1 << UF_PORT)) > 0; }
			bool has_path() const { return (handle_.field_set & (1 << UF_PATH)) > 0; }
			bool has_query() const { return (handle_.field_set & (1 << UF_QUERY)) > 0; }
			bool has_fragment() const { return (handle_.field_set & (1 << UF_FRAGMENT)) > 0; }

		private:
			http_parser_url handle_;
			std::string buf_;
		};

		class response
		{
			friend class client_context;
		private:
			response(client_context* client, Tcp* socket);
				
			~response() {}

		public:
			bool end(const std::string& body);	
			void set_status(int status_code);
			void set_header(const std::string& key, const std::string& value);
			static std::string get_status_text(int status);
		private:
			http_client_ptr client_;
			Tcp* socket_;
			std::map<std::string, std::string, text::ci_less> headers_;
			int status_;
		};

		class request
		{
			friend class client_context;
		private:
			request();

			~request(){}

		public:
			const url_obj& url() const { return url_; }

			const std::string& get_header(const std::string& key) const;

			bool get_header(const std::string& key, std::string& value);

			const std::string& get_body(void) const
			{
				return body_;
			}
		private:
			url_obj url_;
			std::map<std::string, std::string, text::ci_less> headers_;
			std::string body_;
			std::string default_value_;
		};
	}
	
	class Http
	{
	public:
		Http();
		~Http();

	public:
		static std::shared_ptr<Http> create_server(const std::string& ip, int port, std::function<void(http::request&, http::response&)> callback)
		{
			auto server = std::shared_ptr<Http>(new Http);
			if (server->start(ip, port, callback)) return server;
			return nullptr;
		}
	
		bool start(const std::string& ip, int port, std::function<void(http::request&, http::response&)> callback);
	private:
		std::shared_ptr<Tcp> sock_;
	};
}

#endif