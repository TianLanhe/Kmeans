#ifndef LOG_H
#define LOG_H

#include <ostream>
#include <iostream>
#include <vector>

namespace KMeans {

	class Log
	{
	public:
		void add(std::ostream* ost) { m_ostreams.push_back(ost); }
		void clear() {
			for (std::vector<std::ostream*>::size_type i = 0; i < m_ostreams.size(); ++i)
				if (m_ostreams[i] != &std::cout && m_ostreams[i] != &std::cerr)
					delete m_ostreams[i];
			m_ostreams.clear();
		}

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

	extern Log out;

}

#endif // !LOG_H
