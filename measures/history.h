#ifndef HISTORY_H_INCLUDED
#define HISTORY_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include "counter.h"
#include <vector>
#include <functional>
#include <string_view>

namespace perf_monitor
{
	namespace history
	{
		using cref_counter = std::reference_wrapper< const counter > ;

		// peek returns references to the counters which are in history
		std::vector<cref_counter> peek(); // all counters
		std::vector<cref_counter> peek_name( std::string_view name );
		std::vector<cref_counter> peek_name(const std::string& name);
		std::vector<cref_counter> peek_srce(const std::string& srce );
		std::vector<cref_counter> peek_srce(std::string_view srce);

		// extract returns copies of the counters which are also removed from history
		std::vector<counter> extract();  // all counters
		std::vector<counter> extract_name( std::string_view name );
		std::vector<counter> extract_name( const std::string& name);
		std::vector<counter> extract_srce(const std::string& srce);
		std::vector<counter> extract_srce(std::string_view srce);

		// discard removes counters from history
		void discard(); // all counters
		void discard_name(std::string_view name);
		void discard_name(const std::string& name);
		void discard_srce(const std::string& srce);
		void discard_srce(std::string_view srce);

		void push(counter c); // add to history

		void trim_size( std::size_t sz );
		void trim_old( std::time_t t );
	}
}

#endif // HISTORY_H_INCLUDED