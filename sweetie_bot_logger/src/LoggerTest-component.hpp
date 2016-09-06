#ifndef OROCOS_SWEETIE_BOT_LOGGER_COMPONENT_HPP
#define OROCOS_SWEETIE_BOT_LOGGER_COMPONENT_HPP

#include <rtt/RTT.hpp>

#include "logger.hpp"

namespace sweetie_bot {

class LoggerTest : public RTT::TaskContext{
	protected:
		sweetie_bot::LoggerOCL log_ocl;				// via OCL::logger::Category
		sweetie_bot::LoggerLog4Cpp log_log4cpp;		// via log4cpp::Category
		sweetie_bot::LoggerRosout log_rosout;		// via 'rosout' stream using log4cpp::Category
		sweetie_bot::LoggerRosout log_rosout_ocl;	// via 'rosout' stream using OCL::Logger::Category

	public:
		LoggerTest(std::string const& name);

		bool startHook();
		void updateHook();
		void stopHook();
};

} /* namespace sweetie_bot */

#endif
