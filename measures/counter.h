#ifndef COUNTER_H_INCLUDED
#define COUNTER_H_INCLUDED

#include <variant>
#include <string>
#include <string_view>
#include <type_traits>
#include <chrono>
#include "time_utils.h"

namespace perf_monitor
{
	struct counter
	{
		enum type_t { NUMBER = 0, PERCENTAGE = 1, RATE = 2 };

		counter() : counter( "", 0 ) {}

		// name and integer value : NUMBER
		template < typename INT_TYPE >
		counter( std::string_view name, INT_TYPE num, std::string_view srce = "127.0.0.1",
			     typename std::enable_if< std::is_integral_v<INT_TYPE>, void >::type* = nullptr )
			: name(name), info( (unsigned long long)num ), source(srce) {}

		// name and floating point value : PERCENTAGE
		counter( std::string_view name, double percent, std::string_view srce = "127.0.0.1" )
			: name(name), info(percent), source(srce) {}

		// name and rate (count, period) : RATE
		counter( std::string_view name, unsigned long long cnt, unsigned int pm, std::string_view srce = "127.0.0.1" )
			: name(name), info( rate_t{ cnt,pm } ), source(srce) {}

		static unsigned long long next_sl_no;
		unsigned long long sl_no = ++next_sl_no ;
		std::string name;
		std::time_t time = to_time_t();

		struct rate_t
		{
			unsigned long long count;
			unsigned int period_millisecs;

			friend bool operator== ( const rate_t& a, const rate_t& b ) 
			{ return a.count == b.count && a.period_millisecs == b.period_millisecs; }
			friend bool operator!= (const rate_t& a, const rate_t& b) { return !(a == b); }
		};

		std::variant< unsigned long long, double, rate_t > info;

		std::string source = "127.0.0.1";

		type_t type() const { return type_t(info.index()); }
		int index_info() const { return info.index(); }

		std::string to_string() const;

		
	};

	struct cmp_name
	{
		bool operator() (const std::string& a, const std::string& b) const
		{
			return make_key(a) < make_key(b);
		}

		static std::string make_key(const std::string& str)
		{
			return to_lower(trim(str));
		}

		static unsigned char uc(char c) { return c; }

		static std::string to_lower(std::string str)
		{
			for (char& c : str) c = std::tolower(uc(c));
			return str;
		}

		static std::string trim(std::string str)
		{
			std::size_t pos = 0;
			while (pos < str.size() && std::isspace(uc(str[pos]))) ++pos;
			while (!str.empty() && std::isspace(uc(str.back()))) str.pop_back();
			return str.substr(pos);
		}
	};
}

#endif // COUNTER_H_INCLUDED
