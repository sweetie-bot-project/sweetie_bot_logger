#ifndef OROCOS_SWEETIE_BOT_LOGGER_HPP
#define OROCOS_SWEETIE_BOT_LOGGER_HPP

#include <rtt/RTT.hpp>
#include <rtt/Logger.hpp>
#include <rtt/os/Mutex.hpp>

#include <orocos/ocl/Category.hpp>
#include <orocos/ocl/CategoryStream.hpp>

#include <rosgraph_msgs/Log.h>

namespace sweetie_bot {

enum LoggerPriority {
	NOTSET = log4cpp::Priority::NOTSET,
	DEBUG = log4cpp::Priority::DEBUG,
	INFO = log4cpp::Priority::INFO,
	WARN = log4cpp::Priority::WARN,
	ERROR = log4cpp::Priority::ERROR,
	CRIT = log4cpp::Priority::CRIT,
	FATAL = log4cpp::Priority::FATAL,
};

//TODO Where should be this code?
//Convinence macro fo logging.
std::ostream& resetfmt(std::ostream& s) {
	s.copyfmt(std::ios(NULL)); 
	return s;
}


class Logger
{
	public:
		typedef std::ostream& (*LogFunction)(std::ostream&);

		enum LogPriority {
			NOTSET = log4cpp::Priority::NOTSET,
			DEBUG = log4cpp::Priority::DEBUG,
			INFO = log4cpp::Priority::INFO,
			WARN = log4cpp::Priority::WARN,
			ERROR = log4cpp::Priority::ERROR,
			CRIT = log4cpp::Priority::CRIT,
			FATAL = log4cpp::Priority::FATAL,
		};

	protected:
		log4cpp::Category* category;
		log4cpp::Priority::Value priority;
		RTT::rt_ostringstream oss;

	protected:
		Logger() :
			category(0), priority(NOTSET)
		{}

		Logger(const string& category_name) : 
			category(&log4cpp::Category::getInstance(category_name)),
			priority(NOTSET),
			oss("")
		{}

		Logger(log4cpp::Category * _category) : 
			category(_category), priority(NOTSET), oss("")
		{}

	public:

		bool ready() {
			return category;
		}

		bool isPriorityEnabled(LogPriority priority) const { 
			return category && category->isPriorityEnabled(priority);
		}

		bool isPriorityEnabled(LoggerPriority priority) const { return isPriorityEnabled(LogPriority(priority)); }

		void clear() {
			oss.clear();
			oss.seekp(0);
		}

		virtual void flush() {
			oss << std::ends;
			if (category) category->log(priority, oss.str().c_str());
			this->clear();
		}

		operator bool() { 
			return category && priority != NOTSET;
		}

		Logger& operator()() { 
			return *this;	
		}

		Logger& operator()(LogPriority level) { 
			if (priority != level) {
				clear();
				if (category && category->isPriorityEnabled(level)) priority = level;	
				else priority = NOTSET; 
			}
			return *this;	
		}

		Logger& operator()(LoggerPriority level) { this->operator()(LogPriority(level)); }

		template <typename T> Logger& operator<<(const T& t) {
			if (priority != NOTSET) oss << t;
			return *this;
		}

		static std::ostream& endl(std::ostream& __os) { return __os; }
		static std::ostream& discard(std::ostream& __os) { return __os; }

		Logger& operator<<(std::ostream& (*pf)(std::ostream&)) {
			if (priority != NOTSET) {	
				if ( pf == RTT::Logger::endl || pf == endl || pf == RTT::Logger::flush) this->flush();
				else if ( pf == discard) this->clear();
				else oss << pf;
			}
			return *this;
		}

		void shutdown() {
			category = 0;
		}
};

class LoggerOCL : public Logger 
{
	protected:
		OCL::logging::Category * ocl_category;

	public:
		LoggerOCL() : ocl_category(0) {} 
		LoggerOCL(const string& category_name);
		LoggerOCL(log4cpp::Category * _category);

		virtual void flush() {
			oss << std::ends;
			if (ocl_category) ocl_category->log(priority, oss.str());
			this->clear();
		}
};


class LoggerLog4Cpp : public Logger 
{
	public:
		LoggerLog4Cpp() {} 

		LoggerLog4Cpp(const string& category_name);
		LoggerLog4Cpp(log4cpp::Category * _category);
};


class LoggerRosout : public Logger
{
	protected:
		RTT::os::Mutex port_lock; 
		RTT::OutputPort< rosgraph_msgs::Log > rosout_port;
		rosgraph_msgs::Log rosout_msg;

	public:
		LoggerRosout() : rosout_port("rosout") {}

		LoggerRosout(const string& category_name, int buffer_size = 50) : 
			Logger(category_name),
			rosout_port("rosout")
		{
			this->reopenStream(buffer_size);
		}

		LoggerRosout(log4cpp::Category * _category, int buffer_size = 50) : 
			Logger(_category),
			rosout_port("rosout")
		{
			this->reopenStream(buffer_size);
		}

		bool reopenStream(int buffer_size);

		bool ready() {
			return category && rosout_port.connected();
		}

		virtual void flush();
};

//// LOGGER RTT /////

class LoggerRTT : public Logger 
{
	protected:
		string old_module;
	public:
		LoggerRTT() {} 
		LoggerRTT(const string& category_name);
		LoggerRTT(log4cpp::Category * _category) : Logger(_category) {}

		virtual void flush();
};

}

#endif
