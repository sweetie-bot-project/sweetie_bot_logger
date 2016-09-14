#include <sweetie_bot_logger/logger.hpp>

#include <rtt/os/MutexLock.hpp>

#include <orocos/rosgraph_msgs/typekit/Log.h>
#include <orocos/rtt_roscomm/rtt_rostopic.h>

namespace sweetie_bot 
{

//// LOGGER OCL /////

LoggerOCL::LoggerOCL(const string& category_name) :
	ocl_category(dynamic_cast<OCL::logging::Category*>( &log4cpp::Category::getInstance(category_name) )) 
{
	category = ocl_category;
	if (!category) RTT::log(RTT::Error) << "LoggerOCL: category `" << category_name << "' is not OCL::logger::Category. Disable logging." << RTT::endlog();
}

LoggerOCL::LoggerOCL(OCL::logging::Category * _category) : 
	ocl_category(dynamic_cast<OCL::logging::Category*>(_category))
{
	if (category && !ocl_category) RTT::log(RTT::Error) << "LoggerOCL: provided category is not OCL::logger::Category. Disable logging." << RTT::endlog();
	category = ocl_category;
}

//// LOGGER LOG4CPP /////

LoggerLog4Cpp::LoggerLog4Cpp(const string& category_name) : 
	Logger(category_name)
{
	if (dynamic_cast<OCL::logging::Category*>(category)) {
		RTT::log(RTT::Warning) << "LoggerLog4Cpp: category '" << category_name << "' is actually OCL::Category. Use LoggerOCL instead." << RTT::endlog();
		//category = 0;
	}
}

LoggerLog4Cpp::LoggerLog4Cpp(OCL::logging::Category * _category) : 
	Logger(_category)
{
	if (dynamic_cast<OCL::logging::Category*>(category)) {
		RTT::log(RTT::Warning) << "LoggerLog4Cpp: provided category is actually OCL::Category. Use LoggerOCL instead." << RTT::endlog();
		//category = 0;
	}
}

//// LOGGER ROSOUT /////

bool LoggerRosout::reopenStream(int buffer_size) 
{
	if(rosout_port.connected()) rosout_port.disconnect();
	bool success = rosout_port.createStream(rtt_roscomm::topicBuffer("/rosout", buffer_size));
	if (!success) RTT::log(RTT::Error) << "LoggerRosout: unable connect to /rosout topic." << RTT::endlog();
	return success;
}      

void LoggerRosout::flush() {
	oss << std::ends;
	if (category && rosout_port.connected()) {
		// prepare message
		rosout_msg.header.seq++;
		// set ros timestamp
		log4cpp::TimeStamp now;
		rosout_msg.header.stamp.sec = now.getSeconds(); 
		rosout_msg.header.stamp.nsec = 1000 * now.getMicroSeconds(); 

		switch (priority) {
			case log4cpp::Priority::FATAL: //less value
				rosout_msg.level = rosgraph_msgs::Log::FATAL;
				break;
			case log4cpp::Priority::ALERT:
			case log4cpp::Priority::CRIT:
			case log4cpp::Priority::ERROR:
				rosout_msg.level = rosgraph_msgs::Log::ERROR;
				break;
			case log4cpp::Priority::WARN:
				rosout_msg.level = rosgraph_msgs::Log::WARN;
				break;
			case log4cpp::Priority::NOTICE:
			case log4cpp::Priority::INFO:
				rosout_msg.level = rosgraph_msgs::Log::INFO;
				break;
			case log4cpp::Priority::DEBUG: //greater value
				rosout_msg.level = rosgraph_msgs::Log::DEBUG;
				break;
			default:
				return;
		}

		rosout_msg.name = category->getName();
		rosout_msg.msg = oss.str().c_str();

		{
			RTT::os::MutexLock lock(port_lock); 
			// prevent simultenios write attempts
			rosout_port.write(rosout_msg);
		}
	}
	this->clear();
}

}
