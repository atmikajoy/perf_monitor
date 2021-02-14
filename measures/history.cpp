#include "history.h"
#include <map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <iostream>
#include <fstream>
#include "time_utils.h"
#include <mutex>

namespace perf_monitor
{
	namespace
	{
		using namespace boost;
		using namespace boost::multi_index;

		struct by_seq {} ;
		struct by_name {} ;
		struct by_time {} ;
		struct by_srce {};
		using container_type = boost::multi_index_container
			              <
			                 counter,
			                 indexed_by
			                 <
			                    sequenced< tag<by_seq> >,
								ordered_non_unique< tag<by_name>, member< counter, std::string, &counter::name > >,
			                    ordered_non_unique< tag<by_time>, member< counter, std::time_t, &counter::time > >,
			                    ordered_non_unique< tag<by_srce>, member< counter, std::string, &counter::source > >
							 >
		
						  >;

		container_type& container()
		{
			static container_type cntr;
			return cntr;
		}

		std::mutex& lock()
		{
			static std::mutex m;
			return m;
		}

		using lock_guard = std::lock_guard<std::mutex>;

		void debug_dump()
		{
			std::cout << "\n----------------------------\n";
			for ( const auto& cntr : container() ) std::cout << cntr.to_string() << '\n';
		}

		std::string time_stamp()
		{
			const std::tm start_time = calendar_time();
			char temp_buffer[256];
			std::strftime(temp_buffer, sizeof(temp_buffer), "%d%m%Y%H%M%S", &start_time);
			return temp_buffer;
		}

		std::ostream& history_log()
		{
			static const std::string log_file_name = "history/history.log." + time_stamp() + ".txt" ;
			static std::ofstream log_file(log_file_name);
			return log_file;
		}

		std::vector<history::cref_counter> do_peek()
		{
			lock_guard temp(lock());
			std::vector<history::cref_counter> result;
			for (const auto& c : container()) result.push_back(c);
			return result;
		}

		std::vector<counter> do_extract()
		{
			lock_guard temp(lock());
			std::vector<counter> result;
			for (const auto& c : container()) result.push_back(c);
			container().clear();
			return result;
		}

		template < typename TAG, typename KEY >
		std::vector<history::cref_counter> do_peek(const KEY& key)
		{
			lock_guard temp(lock());
			auto& iter = container().get<TAG>();
			return { iter.lower_bound(key), iter.upper_bound(key) };
		}

		template<typename TAG, typename KEY>
		std::vector<counter> do_extract(const KEY& key)
		{
			lock_guard temp(lock());
			auto& iter = container().get<TAG>();
			const auto& from = iter.lower_bound(key);
			const auto& to = iter.upper_bound(key);
			std::vector<counter> result(from, to);
			iter.erase(from, to);
			return result;
		}
		
		template<typename TAG, typename KEY>
		void do_discard(const KEY& key)
		{
			lock_guard temp(lock());
			auto& iter = container().get<TAG>();
			iter.erase(iter.lower_bound(key), iter.upper_bound(key));
		}


	}

	namespace history
	{
		void trim_size(std::size_t sz)
		{
			lock_guard temp( lock() );
			auto& iter = container().get<by_seq>();
			while (container().size() > sz) iter.pop_front();
		}

		void trim_old(std::time_t t)
		{
			lock_guard temp( lock() );
			auto& iter = container().get<by_time>();
			iter.erase(iter.lower_bound(0), iter.upper_bound(t));
		}

		std::vector<cref_counter> peek()
		{
			return do_peek();
		}

		std::vector<counter> extract()
		{
			return do_extract();
		}

		std::vector<cref_counter> peek_name(std::string_view name)
		{
			return peek_name( std::string(name));
		}

		std::vector<cref_counter> peek_name(const std::string& name)
		{
			return do_peek<by_name>(name);
		}

		std::vector<cref_counter> peek_srce(const std::string& srce)
		{
			return do_peek<by_srce>(srce);
		}

		std::vector<cref_counter> peek_srce(std::string_view srce)
		{
			return peek_srce(std::string(srce));
		}

		std::vector<counter> extract_name(std::string_view name)
		{
			return extract_name(std::string(name));
		}

		std::vector<counter> extract_name( const std::string& name)
		{
			return do_extract<by_name>(name);
		}

		std::vector<counter> extract_srce(const std::string& srce)
		{
			return do_extract<by_srce>(srce);
		}

		std::vector<counter> extract_srce(std::string_view srce)
		{
			return extract_srce(std::string(srce));
		}


		void push( counter c )
		{
			lock_guard temp( lock() );
			history_log() << c.to_string() << '\n';
			container().push_back( std::move(c) ) ;
		}

		void discard() 
		{
			lock_guard temp( lock() );
			container().clear(); 
		}

		void discard_name(std::string_view name) { discard_name( std::string(name) ); }

		void discard_name( const std::string& name) 
		{
			do_discard<by_name>(name);
		}

		void discard_srce(std::string_view srce)
		{
			discard_srce(std::string(srce));
		}
		void discard_srce(const std::string& srce)
		{
			do_discard<by_srce>(srce);

		}

	}

#ifdef TEST_HISTORY
	void test_it()
	{

		for (int i = 100; i < 125; ++i)
		{
			container().push_back({ "abcd", i });
			container().push_back({ "efgh", i + 0.5 });
			container().push_back({ "ijkl", i*2ULL, 100 });
		}
		debug_dump();

		std::cin.get();
		std::string name = "abcd";
		history::extract(name);
		debug_dump();

		std::cin.get();
		history::trim_size(12);
		debug_dump();
	}
#endif // TEST_HISTORY
}

#ifdef TEST_HISTORY
int main()
{
	perf_monitor::test_it();
}
#endif // TEST_HISTORY

