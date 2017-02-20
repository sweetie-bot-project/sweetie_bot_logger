#include "LoggerTest-component.hpp"

#include <iostream>
#include <iomanip>

#include <rtt/RTT.hpp>
#include <rtt/Component.hpp>
#include <rtt/Logger.hpp>


using namespace RTT;

namespace sweetie_bot {
using logger::Logger;

LoggerTest::LoggerTest(std::string const& name) :
	TaskContext(name, Stopped),
	log_ocl(logger::getDefaultCategory("orocos") + ".ocl"),
	log_log4cpp(logger::getDefaultCategory("orocos") + ".log4cpp"),
	log_rosout(logger::getDefaultCategory("orocos") + ".rosout", 20),
	log_rosout_ocl(logger::getDefaultCategory("orocos") + ".ocl.rosout", 20),
	log_rtt(logger::getDefaultCategory("orocos") + ".logger_test")
{
	RTT::Logger::In in("LoggetTest");
	//  Loggers state check is mandatory. Constructor reports about error via RTT::Logger.
	if (! log_ocl.ready()) {
		RTT::log(RTT::Error) << "Logger log_ocl is not ready!" << RTT::endlog();
	}
	if (! log_log4cpp.ready()) {
		RTT::log(RTT::Error) << "Logger log_log4cpp is not ready!" << RTT::endlog();
	}
	if (! log_rosout.ready()) {
		RTT::log(RTT::Error) << "Logger log_rosout is not ready!" << RTT::endlog();
	}
	if (! log_rosout_ocl.ready()) {
		RTT::log(RTT::Error) << "Logger log_rosout_ocl is not ready!" << RTT::endlog();
	}
	if (! log_rtt.ready()) {
		RTT::log(RTT::Error) << "Logger log_rtt is not ready!" << RTT::endlog();
	}
	this->provides()->doc("Test component for Great and Powerful Logger from Sweetie Bot project.");
}

bool LoggerTest::startHook()
{
	setPeriod(0.0);
	return true;
}


void LoggerTest::updateHook(){
   RTT::log(RTT::Info) << "This is subtle RTT::Logger..." << RTT::endlog();

  testLogger(log_ocl, "LOG_OCL");
  testLogger(log_log4cpp, "LOG_LOG4CPP");
  testLogger(log_rosout, "LOG_ROSOUT");
  testLogger(log_rosout_ocl, "LOG_ROSOUT_OCL");
  testLogger(log_rtt, "LOG_RTT");
}

void LoggerTest::testLogger(sweetie_bot::logger::Logger & log, const std::string& name) 
{
  log(INFO) << "GREAT AND POWERFUL " << name << " begins!" << RTT::endlog();
  
  log(WARN) << "Come all! " << RTT::Logger::nl;
  log() << "Come all! " << endlog();
  log() << "Do not miss my next message!" << Logger::endl;

  log(DEBUG) << "Do you love GREAT AND POWERFUL " << name << " I love me too!" << Logger::endl;
  if (log(WARN)) {
	  log() << "Let's count with " << name << "! " << std::hex << std::setfill('0');
	  for(int i = 0; i < 16; i++) log(WARN) << std::setw(2) << i << " ";
	  log() << Logger::endl << "New line" << Logger::endl;
  }
  log(ERROR) << "AAA! Its Ursa Major!!!" << RTT::Logger::endl;
  log(CRIT) << "Show must go on!" << RTT::Logger::endl;

  log(FATAL) << "Well, I'm not so great..." << Logger::discard;
  log(FATAL) << "Well, I'm not so great...";
  
  log(INFO) << "Come and witness amazing log of GREAT AND POWERFUL " << name << "!" << endlog();
}


void LoggerTest::stopHook() {
}

} 

/*
 * Using this macro, only one component may live
 * in one library *and* you may *not* link this library
 * with another component library. Use
 * ORO_CREATE_COMPONENT_TYPE()
 * ORO_LIST_COMPONENT_TYPE(LoggerTest)
 * In case you want to link with another library that
 * already contains components.
 *
 * If you have put your component class
 * in a namespace, don't forget to add it here too:
 */
ORO_CREATE_COMPONENT(sweetie_bot::LoggerTest)
