#include "nw_message.h"
#include <iostream>
#include <sstream>

namespace perf_monitor
{
	namespace
	{
		// enum type_t { NUMBER = 0, PERCENTAGE = 1, RATE = 2 };
		const std::string type_str[] = { "NUMBER", "PERCENTAGE", "RATE" };
	}
	nw_message::nw_message(const counter& cntr)
	{
		auto i = cntr.info.index();
		text = std::to_string(cntr.sl_no) + ';'
			+ cntr.source + ';'
			+ type_str[cntr.type()] + ';'
			+ cntr.name + ';'
			+ std::to_string(cntr.time) + ';' ;

		switch (cntr.type())
		{
			case counter::NUMBER:
				text += std::to_string(std::get<unsigned long long>(cntr.info));
				break;

			case counter::PERCENTAGE:
				text += std::to_string(std::get<double>(cntr.info));
				break;

			case counter::RATE:
			{
				const auto& r = std::get<counter::rate_t>(cntr.info);
				text += std::to_string(r.count) + ';' + std::to_string(r.period_millisecs);
			}
		}

		text += ';';
		text.resize(FIXED_SIZE);
	}

	nw_message::operator counter() const
	{
		std::istringstream stm(text);
		counter cntr;
		
		std::string fld;
		std::getline(stm, fld, ';'); cntr.sl_no = std::stoull(fld);

		std::getline(stm, fld, ';'); cntr.source = fld;

		std::getline(stm, fld, ';'); 
		int i = counter::NUMBER ; 
		for ( ; i <= counter::RATE; ++i ) if (type_str[i] == fld) break;
		const auto ty = counter::type_t(i);

		std::getline(stm, cntr.name, ';'); 
		std::getline(stm, fld, ';'); cntr.time = std::stoull(fld);

		switch (ty)
		{
			case counter::NUMBER:
				std::getline(stm, fld, ';'); cntr.info = std::stoull(fld);
				break;

			case counter::PERCENTAGE:
				std::getline(stm, fld, ';'); cntr.info = std::stod(fld);
				break;
				break;

			case counter::RATE:
			{
				std::getline(stm, fld, ';');
				const auto cnt = std::stoull(fld);
				std::getline(stm, fld, ';');
				const unsigned int period = std::stoul(fld);
				cntr.info = counter::rate_t{ cnt, period };
			}
		}

		return cntr;
	}
}
