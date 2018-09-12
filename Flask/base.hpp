#pragma once

#include <map>
#include <string>
#include <iostream>
#include <regex>

namespace Flask
{
	using path_vars = std::map < std::string, std::string >;
	
	const std::map < std::string, std::string > codes_reason {{"100", "Continue"}, {"101", "Switching Protocols"}, {"200", "OK"}, {"201", "Created"}, {"202", "Accepted"}, {"203", "Non-Authoritative Information"}, {"204", "No Content"}, {"205", "Reset Content"}, {"206", "Partial Content"}, {"300", "Multiple Choices"}, {"301", "Moved Permanently"}, {"302", "Found"}, {"303", "See Other"}, {"304", "Not Modified"}, {"305", "Use Proxy"}, {"307", "Temporary Redirect"}, {"400", "Bad Request"}, {"401", "Unauthorized"}, {"402", "Payment Required"}, {"403", "Forbidden"}, {"404", "Not Found"}, {"405", "Method Not Allowed"}, {"406", "Not Acceptable"}, {"407", "Proxy Authentication Required"}, {"408", "Request Time-out"}, {"409", "Conflict"}, {"410", "Gone"}, {"411", "Length Required"}, {"412", "Precondition Failed"}, {"413", "Request Entity Too Large"}, {"414", "Request-URI Too Large"}, {"415", "Unsupported Media Type"}, {"416", "Requested range not satisfiable"}, {"417", "Expectation Failed"}, {"500", "Internal Server Error"}, {"501", "Not Implemented"}, {"502", "Bad Gateway"}, {"503", "Service Unavailable"}, {"504", "Gateway Time-out"}, {"505", "HTTP Version not supported"}};

	struct status_line
	{
	public:
	private:
		std::string http;
		std::string code;
	public:
		status_line (std::string _http, std::string _code) : http (_http), code (_code) 
		{}

		std::string to_string () const
		{
			return "HTTP-" + this->http + " " + this->code + " " + (codes_reason.find (this->code)->second) + "\n\r";
		}
	};

	struct response 
	{
		public:

		private:
			status_line status;
			std::map < std::string, std::string > header;
			std::string body;
		public:
			response (status_line s = status_line ("1.1", "204"), std::map < std::string, std::string > m = {}, std::string _body = "") : status (s), header (m), body (_body)
			{}
			std::string to_string () const
			{
				std::string res = status.to_string ();
				for (auto& x : header)
					res += x.first + ": " + x.second + "\n\r";
				res += "\n\r";
				res += body;
				return std::move (res);
			}
	};
/*static map < unsigned short, response > responses = 
{"100", "Continue"},
{"101", "Switching Protocols"},
{"200", "OK"},
{"201", "Created"},
{"202", "Accepted"},
{"203", "Non-Authoritative Information"},
{"204", "No Content"},
{"205", "Reset Content"},
{"206", "Partial Content"},
{"300", "Multiple Choices"},
{"301", "Moved Permanently"},
{"302", "Found"},
{"303", "See Other"},
{"304", "Not Modified"},
{"305", "Use Proxy"},
{"307", "Temporary Redirect"},
{"400", "Bad Request"},
{"401", "Unauthorized"},
{"402", "Payment Required"},
{"403", "Forbidden"},
{"404", "Not Found"},
{"405", "Method Not Allowed"},
{"406", "Not Acceptable"},
{"407", "Proxy Authentication Required"},
{"408", "Request Time-out"},
{"409", "Conflict"},
{"410", "Gone"},
{"411", "Length Required"},
{"412", "Precondition Failed"},
{"413", "Request Entity Too Large"},
{"414", "Request-URI Too Large"},
{"415", "Unsupported Media Type"},
{"416", "Requested range not satisfiable"},
{"417", "Expectation Failed"},
{"500", "Internal Server Error"},
{"501", "Not Implemented"},
{"502", "Bad Gateway"},
{"503", "Service Unavailable"},
{"504", "Gateway Time-out"},
{"505", "HTTP Version not supported"}
; */
}
