#ifndef OROCOS_SWEETIE_BOT_LOGGER_HPP
#define OROCOS_SWEETIE_BOT_LOGGER_HPP

#include <rtt/RTT.hpp>
#include <rtt/Logger.hpp>
#include <rtt/os/Mutex.hpp>

#include <orocos/ocl/Category.hpp>
#include <orocos/ocl/CategoryStream.hpp>

#include <rosgraph_msgs/Log.h>

namespace sweetie_bot {

/**
 * @brief Log4cpp logger priorities.
 **/
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

namespace logger {

/**
 * @brief Logger base virtual class.
 * Provides basic logger functionality.
 **/
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

		Logger(const std::string& category_name) : 
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

/**
 * @brief OCL log4cpp logger.
 * Messages are feedforwarded throught OROCOS port to OCL::logger::Appender components.
 * OCL::logger::LoggerService component is necesssary to deploy logging infrastructure.
 **/
class LoggerOCL : public Logger 
{
	protected:
		OCL::logging::Category * ocl_category;

	public:
		LoggerOCL() : ocl_category(0) {} 
		LoggerOCL(const std::string& category_name);
		LoggerOCL(log4cpp::Category * _category);

		virtual void flush() {
			oss << std::ends;
			if (ocl_category) ocl_category->log(priority, oss.str());
			this->clear();
		}
};

/**
 * @brief Plain log4cpp logger.
 * Use log4cpp facilities to produce log.
 **/
class LoggerLog4Cpp : public Logger 
{
	public:
		LoggerLog4Cpp() {} 

		LoggerLog4Cpp(const std::string& category_name);
		LoggerLog4Cpp(log4cpp::Category * _category);
};

/**
 * @brief Log to /rosuot directly.
 * Feedforward log message to /rosout. 
 **/
class LoggerRosout : public Logger
{
	protected:
		RTT::os::Mutex port_lock; 
		RTT::OutputPort< rosgraph_msgs::Log > rosout_port;
		rosgraph_msgs::Log rosout_msg;

	public:
		LoggerRosout() : rosout_port("rosout") {}

		LoggerRosout(const std::string& category_name, int buffer_size = 50) : 
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

/**
 * @brief Log via RTT::Logger.
 * Log via RTT::Logger. Log4cpp category priorities still are applied, so log level can be confugured per component.
 **/
class LoggerRTT : public Logger 
{
	protected:
		std::string old_module;
	public:
		LoggerRTT() {} 
		LoggerRTT(const std::string& category_name);
		LoggerRTT(log4cpp::Category * _category) : Logger(_category) {}

		virtual void flush();
};

//// Helper functions

/**
 * @brief Get category from log4cpp Service property.
 * Return default_category property of log4cpp service. It is assumed the service is loaded into GlobalService.
 * @param default_category Default category if log4cpp service does not presents.
 * @return Category name.
 **/
std::string getDefaultCategory(const std::string& default_category = "");

} // namespace logger
} // namespace 

#endif
