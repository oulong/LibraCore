#include "http.h"
#include "error.h"
#include "http_parser.h"


#include <sstream>

namespace Libra
{
	namespace http
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		response::response(client_context* client, Tcp* socket)
			: client_(client)
			, socket_(socket)
			, headers_()
			, status_(200)
		{
			headers_["Content-Type"] = "text/html";
		}

		bool response::end(const std::string& body)
		{
			// Content-Length
			if (headers_.find("Content-Length") == headers_.end())
			{
				std::stringstream ss;
				ss << body.length();
				headers_["Content-Length"] = ss.str();
			}

			std::stringstream response_text;
			response_text << "HTTP/1.1 ";
			response_text << status_ << " " << get_status_text(status_) << "\r\n";
			for (auto h : headers_)
			{
				response_text << h.first << ": " << h.second << "\r\n";
			}
			response_text << "\r\n";
			response_text << body;

			auto str = response_text.str();
			return socket_->write(str.c_str(), static_cast<int>(str.length()), [=](const UVError& e) {
				if (e)
				{
					// TODO: handle error
				}
				// clean up
				client_.reset();
			});
		}

		void response::set_status(int status_code)
		{
			status_ = status_code;
		}

		void response::set_header(const std::string& key, const std::string& value)
		{
			headers_[key] = value;
		}

		std::string response::get_status_text(int status)
		{
			switch (status)
			{
			case 100: return "Continue";
			case 101: return "Switching Protocols";
			case 200: return "OK";
			case 201: return "Created";
			case 202: return "Accepted";
			case 203: return "Non-Authoritative Information";
			case 204: return "No Content";
			case 205: return "Reset Content";
			case 206: return "Partial Content";
			case 300: return "Multiple Choices";
			case 301: return "Moved Permanently";
			case 302: return "Found";
			case 303: return "See Other";
			case 304: return "Not Modified";
			case 305: return "Use Proxy";
			//case 306: return "(reserved)";
			case 307: return "Temporary Redirect";
			case 400: return "Bad Request";
			case 401: return "Unauthorized";
			case 402: return "Payment Required";
			case 403: return "Forbidden";
			case 404: return "Not Found";
			case 405: return "Method Not Allowed";
			case 406: return "Not Acceptable";
			case 407: return "Proxy Authentication Required";
			case 408: return "Request Timeout";
			case 409: return "Conflict";
			case 410: return "Gone";
			case 411: return "Length Required";
			case 412: return "Precondition Failed";
			case 413: return "Request Entity Too Large";
			case 414: return "Request-URI Too Long";
			case 415: return "Unsupported Media Type";
			case 416: return "Requested Range Not Satisfiable";
			case 417: return "Expectation Failed";
			case 500: return "Internal Server Error";
			case 501: return "Not Implemented";
			case 502: return "Bad Gateway";
			case 503: return "Service Unavailable";
			case 504: return "Gateway Timeout";
			case 505: return "HTTP Version Not Supported";
			default: throw response_exception("Not supported status code.");
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////		
		request::request()
				: url_()
				, headers_()
				, body_("")
		{
		}
	
		const std::string& request::get_header(const std::string& key) const
		{
			auto it = headers_.find(key);
			if (it != headers_.end()) return it->second;
			return default_value_;
		}

		bool request::get_header(const std::string& key, std::string& value)
		{
			auto it = headers_.find(key);
			if (it != headers_.end())
			{
				value = it->second;
				return true;
			}
			return false;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
        class Http;
		class client_context
		{
			friend class Http;

        public:
			client_context(Tcp* server)
				: parser_()
                , parser_settings_()
				, was_header_value_(true)
				, last_header_field_()
				, last_header_value_()
                , socket_(nullptr)
				, request_(nullptr)
				, response_(nullptr)
				, callback_lut_(new callbacks(1))
			{
				//printf("request() %x callback_=%x\n", this, callback_);
				assert(server);

				// TODO: check error
				socket_ = std::make_shared<Tcp>();				
				server->accept(socket_.get());
			}

		public:
			~client_context()
			{
				if (request_)
				{
					delete request_;
					request_ = nullptr;
				}

				if (response_)
				{
					delete response_;
					response_ = nullptr;
				}

				if (callback_lut_)
				{
					delete callback_lut_;
					callback_lut_ = nullptr;
				}

				if (socket_.use_count())
				{
					socket_->close([=](){});
				}
			}

		public:
			bool parse(std::function<void(request&, response&)> callback)
			{
				request_ = new request;
				response_ = new response(this, socket_.get());

				http_parser_init(&parser_, HTTP_REQUEST);
				parser_.data = this;

				// store callback object
				callbacks::store(callback_lut_, 0, callback);

				parser_settings_.on_url = [](http_parser* parser, const char *at, size_t len) {
					auto client = reinterpret_cast<client_context*>(parser->data);

					//  TODO: from_buf() can throw an exception: check
					client->request_->url_.from_buf(at, len);

					return 0;
				};
				parser_settings_.on_header_field = [](http_parser* parser, const char* at, size_t len) {
					auto client = reinterpret_cast<client_context*>(parser->data);

					if (client->was_header_value_)
					{
						// new field started
						if (!client->last_header_field_.empty())
						{
							// add new entry
							client->request_->headers_[client->last_header_field_] = client->last_header_value_;
							client->last_header_value_.clear();
						}

						client->last_header_field_ = std::string(at, len);
						client->was_header_value_ = false;
					}
					else
					{
						// appending
						client->last_header_field_ += std::string(at, len);
					}
					return 0;
				};
				parser_settings_.on_header_value = [](http_parser* parser, const char* at, size_t len) {
					auto client = reinterpret_cast<client_context*>(parser->data);

					if (!client->was_header_value_)
					{
						client->last_header_value_ = std::string(at, len);
						client->was_header_value_ = true;
					}
					else
					{
						// appending
						client->last_header_value_ += std::string(at, len);
					}
					return 0;
				};
				parser_settings_.on_headers_complete = [](http_parser* parser) {
					auto client = reinterpret_cast<client_context*>(parser->data);

					// add last entry if any
					if (!client->last_header_field_.empty()) {
						// add new entry
						client->request_->headers_[client->last_header_field_] = client->last_header_value_;
					}

					return 0; // 1 to prevent reading of message body.
				};
				parser_settings_.on_body = [](http_parser* parser, const char* at, size_t len) {
					//printf("on_body, len of 'char* at' is %d\n", len);
					auto client = reinterpret_cast<client_context*>(parser->data);
					client->request_->body_ = std::string(at, len);
					return 0;
				};
				parser_settings_.on_message_complete = [](http_parser* parser) {
					//printf("on_message_complete, so invoke the callback.\n");
					auto client = reinterpret_cast<client_context*>(parser->data);
					// invoke stored callback object
					callbacks::invoke<std::function<void(request&, response&)>>(
						client->callback_lut_, 0, *client->request_, *client->response_);
					return 1; // 0 or 1?
				};

				socket_->read_start([=](const char* buf, int len){
					if (buf == 0x00 && len == -1) {
						response_->set_status(500);
					}
					else {
						http_parser_execute(&parser_, &parser_settings_, buf, len);
					}
				});

				return true;
			}

		private:
			http_parser parser_;
			http_parser_settings parser_settings_;
			bool was_header_value_;
			std::string last_header_field_;
			std::string last_header_value_;

			std::shared_ptr<Tcp> socket_;
			request* request_;
			response* response_;

			callbacks* callback_lut_;
		};
	}


	Http::Http()
		: sock_(std::make_shared<Tcp>())
	{
		
	}

	Http::~Http()
	{
		if (sock_)
		{
			sock_->close([](){});
		}
	}

	bool Http::start(const std::string& ip, int port, std::function<void(http::request&, http::response&)> callback)
	{		
		if (!sock_->bind(ip, port)) return false;

		if (!sock_->listen([=](const UVError& error){
			if (error)
			{
				std::cout << "listen err: " << error.str() << std::endl;
			}
			else
			{
				auto client = new http::client_context(sock_.get());
				client->parse(callback);
			}
		})) return false;

		return true;
	}
}