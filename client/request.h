#include "counter.h"

namespace perf_monitor
{
	struct request
	{
		enum type_t { BY_NAME = 0, BY_SRCE = 1 };
		type_t type;
		std::string name; // "*" for all
		unsigned long long start_sl_no = 0;
		std::time_t start_time = 0;

		static constexpr std::size_t STRING_LENGTH = 128;
		std::string to_string() const;
		static request from_string(const std::string& req_txt);
	};

	struct reply
	{
		static constexpr std::size_t STRING_LENGTH = 128;
		static const std::string eot;
		static std::string to_string(const counter& cntr);
		static const char* eot_msg();
	};
}