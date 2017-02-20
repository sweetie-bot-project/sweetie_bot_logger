#ifndef OROCOS_SWEETIE_BOT_LOGGER_COMPONENT_HPP
#define OROCOS_SWEETIE_BOT_LOGGER_COMPONENT_HPP

#include <rtt/RTT.hpp>

#include <sweetie_bot_logger/logger.hpp>

namespace sweetie_bot {

class LoggerTest : public RTT::TaskContext{
	protected:
		logger::LoggerOCL log_ocl;				// via OCL::logger::Category
		logger::LoggerLog4Cpp log_log4cpp;		// via log4cpp::Category
		logger::LoggerRosout log_rosout;		// via 'rosout' stream using log4cpp::Category
		logger::LoggerRosout log_rosout_ocl;	// via 'rosout' stream using OCL::Logger::Category
		logger::LoggerRTT log_rtt;				// via RTT::log facility

		void testLogger(sweetie_bot::logger::Logger & log, const std::string& name);
	public:
		LoggerTest(std::string const& name);

		bool startHook();
		void updateHook();
		void stopHook();
};

} /* namespace sweetie_bot */

#endif
