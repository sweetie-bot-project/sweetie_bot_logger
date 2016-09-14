#ifndef  ROSAPPENDER_HH
#define  ROSAPPENDER_HH

#include <log4cpp/AppenderSkeleton.hh>
#include <orocos/rosgraph_msgs/typekit/Log.h>
#include <rtt/OutputPort.hpp>

using namespace RTT;

namespace log4cpp {

    /**
     * RosAppender publish LoggingEvents to rosout ROS topic.
     **/
    //class LOG4CPP_EXPORT RosAppender : public AppenderSkeleton {
    class  RosAppender : public AppenderSkeleton {
		public:
			RosAppender(const std::string& name, int buffer_size);
			~RosAppender();

			virtual bool reopen();
			virtual void close();
			virtual bool requiresLayout() const { return false; }
			virtual void setLayout(Layout* layout) {}

		protected:
			virtual void _append(const LoggingEvent& event);

			RTT::OutputPort< rosgraph_msgs::Log > rosout_port;
			rosgraph_msgs::Log rosout_msg;
			int buffer_size;
    };
}

#endif  /*ROSAPPENDER_HH*/
