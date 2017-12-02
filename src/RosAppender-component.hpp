#ifndef	OSTREAMAPPENDER_HPP
#define	OSTREAMAPPENDER_HPP 1

#include <rtt/Property.hpp>
#include <orocos/ocl/Appender.hpp>

class RosAppender : public OCL::logging::Appender
{
public:
	RosAppender(std::string name);
	virtual ~RosAppender();

protected:
	/// Create log4cpp appender
    virtual bool configureHook();
	/// Process at most one (1) event
	virtual void updateHook();
	/// Destroy appender
	virtual void cleanupHook();

    /** 
     * Maximum number of log events to pop per cycle
     *
     * Defaults to 1.
     *
     * A value of 0 indicates to not limit the number of events per cycle.
     * With enough event production, this could lead to thread
     * starvation!
     */
    int maxEventsPerCycle;
};


#endif
