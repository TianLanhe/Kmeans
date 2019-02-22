#ifndef LOG_H
#define LOG_H

#include <ostream>
#include <vector>

namespace KMeans {

	class Log
	{
	public:
		void add(std::ostream* ost) { m_ostreams.push_back(ost); }
		void clear() { m_ostreams.clear(); }

		template< typename T >
		Log& operator<<(const T& t) {
			for (std::vector<std::ostream*>::size_type i = 0; i < m_ostreams.size(); ++i)
				(*m_ostreams[i]) << t;
			return *this;
		}

		Log& operator<<(std::basic_ostream<char, std::char_traits<char> >& (*func)(std::basic_ostream<char, std::char_traits<char> >&)) {
			for (std::vector<std::ostream*>::size_type i = 0; i < m_ostreams.size(); ++i)
				(*m_ostreams[i]) << std::endl;
			return *this;
		}

	private:
		std::vector<std::ostream*> m_ostreams;
	};

	extern Log log;
	extern Log out;

}

#endif // !LOG_H
