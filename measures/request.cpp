#include "request.h"
#include <sstream>

namespace perf_monitor
{
	namespace
	{
		const std::string request_text[]{ "by_name", "by_srce" };

		const std::string& req_type_to_text(request::type_t type)
		{
			if( type == request::BY_NAME || type == request::BY_SRCE )
				return request_text[type];

			static const std::string invalid = "invalid";
			return invalid;
		}

		request::type_t text_to_req_type(const std::string& str)
		{
			if (str == request_text[request::BY_NAME]) return request::BY_NAME;
			else return request::BY_SRCE;
		}
	}

	std::string request::to_string() const
	{
		std::ostringstream stm;
		stm << req_type_to_text(type) << ';' << name << ';' 
			<< start_sl_no << ';' << start_time;
		std::string txt = stm.str();
		txt.resize(STRING_LENGTH);
		return txt;
	}

	request request::from_string( const std::string& req_txt )
	{
		std::istringstream stm(req_txt);

		std::string type_str;
		std::getline(stm, type_str, ';');
		std::string name ;
		std::getline(stm, name, ';' );
		std::string slno;
		std::getline(stm, slno, ';' );
		std::string time;
		std::getline(stm, time);

		return { text_to_req_type(type_str), name, std::stoull(slno), std::time_t(std::stoull(time)) };
	}

	std::string reply::to_string(const counter& cntr)
	{
		std::string text = cntr.to_string();
		text.resize(STRING_LENGTH);
		return text;
	}

	const std::string reply::eot = "NIL";

	const char* reply::eot_msg()
	{
		static std::string nothing = eot + std::string(STRING_LENGTH,0) ;
		return nothing.c_str() ;
	}
}
