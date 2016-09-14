#include <sweetie_bot_logger/RosAppender.hpp>

#include <stdexcept>
#include <orocos/rtt_roscomm/rtt_rostopic.h>


using namespace RTT;

namespace log4cpp {

    RosAppender::RosAppender(const std::string& name, int _buffer_size) : 
        AppenderSkeleton(name),
        rosout_port("rosout"),
		buffer_size(_buffer_size)
	{
		if (!rosout_port.createStream(rtt_roscomm::topicBuffer("/rosout", buffer_size))) {
			throw std::runtime_error("RosAppender: createStream() to `/rosout' topic failed.");
		}
		rosout_msg.header.seq = 0;
    }
    
    void RosAppender::close() {
		rosout_port.disconnect();
    }

    void RosAppender::_append(const LoggingEvent& event) {
		if (rosout_port.connected()) {
			// prepare message
			rosout_msg.header.seq++;
			rosout_msg.header.stamp.sec = event.timeStamp.getSeconds(); 
			rosout_msg.header.stamp.nsec = 1000 * event.timeStamp.getMicroSeconds(); 

			switch (event.priority) {
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
			
			rosout_msg.name = event.categoryName;
			rosout_msg.msg = event.message;

			rosout_port.write(rosout_msg);
        }
    }

    bool RosAppender::reopen() {
		rosout_port.disconnect();
		return rosout_port.createStream(rtt_roscomm::topicBuffer("/rosout", buffer_size));
    }      

	RosAppender::~RosAppender() {
		// prevent deadlock due to logging attempt
		assert( ! rosout_port.connected() ); // RosAppender: deadlock othervise
	}
}

