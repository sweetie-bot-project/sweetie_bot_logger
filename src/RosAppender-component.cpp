#include "RosAppender-component.hpp"

#include <stdexcept>
#include <rtt/Component.hpp>

#include <sweetie_bot_logger/RosAppender.hpp>

using namespace RTT;

RosAppender::RosAppender(std::string name) :
    OCL::logging::Appender(name),
    maxEventsPerCycle(10)
{
    this->addProperty("MaxEventsPerCycle", maxEventsPerCycle)
		.doc("Maximum number of log events to pop per cycle.")
		.set(10);
}

RosAppender::~RosAppender()
{
}

bool RosAppender::configureHook()
{
    // verify valid limits
    if ((0 > maxEventsPerCycle))
    {
        log(Error) << "Invalid maxEventsPerCycle value of " << maxEventsPerCycle << ". Value must be >= 0." << endlog();
        return false;
    }

	try {
		if (!appender)
			appender = new log4cpp::RosAppender(getName(), maxEventsPerCycle + 2);
	}
	catch (std::runtime_error& e) {
		RTT::Logger::In in("RosAppender");
		RTT::log(Error) << e.what() << endlog();
		appender = 0;
		return false;
	}

    return configureLayout();
}

void RosAppender::updateHook()
{
	processEvents(maxEventsPerCycle);
}

void RosAppender::cleanupHook()
{
    /* normally in log4cpp the category owns the appenders and deletes them
     itself, however we don't associate appenders and categories in the
     same manner. Hence, you have to manually manage appenders.
     */
	appender->close();
    delete appender;
    appender = 0;
}

ORO_CREATE_COMPONENT(RosAppender)
