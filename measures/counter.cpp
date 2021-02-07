#include "counter.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>

namespace perf_monitor
{
	namespace
	{
		static constexpr char counter_sl_no_file_path[] = "cntr_slno.txt";

		unsigned long long init_counter()
		{
			struct init_helper
			{
				init_helper()
				{
					if (std::ifstream file{ counter_sl_no_file_path }) file >> n;
					else n = 0;
				}

				~init_helper()
				{
					std::ofstream(counter_sl_no_file_path) << counter::next_sl_no << '\n';
				}

				unsigned long long n = 0;
			};
			
			static const init_helper helper;

			return helper.n;
		}
	}

	unsigned long long counter::next_sl_no = init_counter() ;

	std::string counter::to_string() const
	{
		std::ostringstream stm;
		stm << sl_no << ". " << source << " : " << name << " : ";

		switch (type())
		{
		   case NUMBER :
			   stm << std::get<NUMBER>(info);
			   break;
           
		   case PERCENTAGE:
			   stm << std::fixed << std::setprecision(2) 
				   << std::get<PERCENTAGE>(info) << '%' ;
			   break;

		   case RATE:
			   {
				   const auto& r = std::get<RATE>(info);
				   double cnt = double(r.count);
				   double secs = r.period_millisecs / 1000.0 ;
				   stm << std::fixed << std::setprecision(2)
					   << cnt / secs << " per second";
			   }
			   break;
		}

		stm << " (" << time << ' ' << str_time(time) << ')' ;
		return stm.str();
	}
}