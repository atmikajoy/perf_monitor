#ifndef NW_MESSAGE_H_INCLUDED
#define NW_MESSAGE_H_INCLUDED

#include "counter.h"
#include <string>

namespace perf_monitor
{
	struct nw_message
	{
		static constexpr std::size_t FIXED_SIZE = 128;
		std::string text; // sl_no;source;type;name;time;info 

		explicit nw_message(const counter& cntr);
		explicit nw_message(const std::string& txt) : text(txt) {}

		operator counter() const;

		static counter text_to_counter(const std::string& txt) { return nw_message(txt); }
	};
}

#endif // NW_MESSAGE_H_INCLUDED