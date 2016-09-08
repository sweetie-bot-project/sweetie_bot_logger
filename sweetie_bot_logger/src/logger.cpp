#include "logger.hpp"

#include <rtt/os/MutexLock.hpp>

#include <orocos/rosgraph_msgs/typekit/Log.h>
#include <orocos/rtt_roscomm/rtt_rostopic.h>

namespace sweetie_bot {

	bool LoggerRosout::reopenStream(int buffer_size) {
		if(rosout_port.connected()) rosout_port.disconnect();
		return rosout_port.createStream(rtt_roscomm::topicBuffer("/rosout", buffer_size));
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
