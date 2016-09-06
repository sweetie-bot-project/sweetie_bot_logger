#include "LoggerTest-component.hpp"

#include <iostream>
#include <iomanip>

#include <rtt/RTT.hpp>
#include <rtt/Component.hpp>
#include <rtt/Logger.hpp>

using namespace RTT;
using sweetie_bot::Logger;

namespace sweetie_bot {

LoggerTest::LoggerTest(std::string const& name) :
	TaskContext(name, Stopped),
	log_ocl("orocos.ocl"),
	log_log4cpp("orocos.log4cpp"),
	log_rosout("orocos.rosout", 20),
	log_rosout_ocl("orocos.ocl.rosout", 20)
{
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
}

bool LoggerTest::startHook()
{
	setPeriod(0.0);
	return true;
}


void LoggerTest::updateHook(){
   RTT::log(RTT::Info) << "This is subtle RTT::Logger..." << RTT::endlog();

  // OCL::loggin::Category
  log_ocl(INFO) << "But now... GREAT AND POWERFUL LOGGER_OCL begins!" << RTT::endlog();
  
  log_ocl(WARN) << "Come all! " << RTT::Logger::nl;
  log_ocl() << "Come all! " << endlog();

  log_ocl(DEBUG) << "Do you love GREAT AND POWERFUL LOGGER_OCL? I love me too!" << Logger::endl;
  if (log_ocl(WARN)) {
	  log_ocl() << "Let's count with LOGGER_OCL! " << std::hex << std::setfill('0');
	  for(int i = 0; i < 16; i++) log_ocl(WARN) << std::setw(2) << i << " ";
	  log_ocl() << Logger::endl << "New line" << Logger::endl;
  }
  log_ocl(ERROR) << "AAA! Its Ursa Major!!!" << RTT::Logger::endl;
  log_ocl(CRIT) << "Show must go on!" << RTT::Logger::endl;

  log_ocl() << "Do not miss my next message!" << Logger::endl;
  
  log_ocl(FATAL) << "Well, I'm not so great..." << Logger::discard;
  log_ocl(FATAL) << "Well, I'm not so great...";

  log_ocl(INFO) << "Come and witness amazing log of GREAT AND POWERFUL LOGGER_OCL!" << endlog();

  // log4cpp::Category
  log_log4cpp(INFO) << "GREAT AND POWERFUL LOGGER_LOG4CPP begins!" << RTT::endlog();
  
  log_log4cpp(WARN) << "Come all! " << RTT::Logger::nl;
  log_log4cpp() << "Come all! " << endlog();
  log_log4cpp(DEBUG) << "Do you love GREAT AND POWERFUL LOGGER_LOG4CPP? I love me too!" << Logger::endl;

  if (log_log4cpp(WARN)) {
	  log_log4cpp() << "Let's count with LOGGER_LOG4CPP! " << std::hex << std::setfill('0');
	  for(int i = 0; i < 16; i++) log_log4cpp(WARN) << std::setw(2) << i << " ";
	  log_log4cpp() << Logger::endl << "New line" << Logger::endl;
  }
  log_log4cpp(ERROR) << "AAA! Its Ursa Major!!!" << RTT::Logger::endl;
  log_log4cpp(CRIT) << "Show must go on!" << RTT::Logger::endl;

  log_log4cpp() << "Do not miss my next message!" << Logger::endl;

  log_log4cpp(FATAL) << "Well, I'm not so great..." << Logger::discard;
  log_log4cpp(FATAL) << "Well, I'm not so great...";
  
  log_log4cpp(INFO) << "Come and witness amazing log of GREAT AND POWERFUL LOGGER_LOG4CPP!" << endlog();

  // rosout via log4cpp::Category
  log_rosout(INFO) << "GREAT AND POWERFUL LOGGER_ROSOUT begins!" << RTT::endlog();
  
  log_rosout(WARN) << "Come all! " << RTT::Logger::nl;
  log_rosout() << "Come all! " << endlog();
  log_rosout() << "Do not miss my next message!" << Logger::endl;

  log_rosout(DEBUG) << "Do you love GREAT AND POWERFUL LOGGER_ROSOUT? I love me too!" << Logger::endl;
  if (log_rosout(WARN)) {
	  log_rosout() << "Let's count with LOGGER_ROSOUT! " << std::hex << std::setfill('0');
	  for(int i = 0; i < 16; i++) log_rosout(WARN) << std::setw(2) << i << " ";
	  log_rosout() << Logger::endl << "New line" << Logger::endl;
  }
  log_rosout(ERROR) << "AAA! Its Ursa Major!!!" << RTT::Logger::endl;
  log_rosout(CRIT) << "Show must go on!" << RTT::Logger::endl;


  log_rosout(FATAL) << "Well, I'm not so great..." << Logger::discard;
  log_rosout(FATAL) << "Well, I'm not so great...";
  
  log_rosout(INFO) << "Come and witness amazing log of GREAT AND POWERFUL LOGGER_ROSOUT!" << endlog();
  
  // log4cpp::Category
  log_rosout_ocl(INFO) << "GREAT AND POWERFUL LOGGER_ROSOUT_OCL begins!" << RTT::endlog();
  
  log_rosout_ocl(WARN) << "Come all! " << RTT::Logger::nl;
  log_rosout_ocl() << "Come all! " << endlog();
  log_rosout_ocl() << "Do not miss my next message!" << Logger::endl;

  log_rosout_ocl(DEBUG) << "Do you love GREAT AND POWERFUL LOGGER_ROSOUT_OCL? I love me too!" << Logger::endl;
  if (log_rosout_ocl(WARN)) {
	  log_rosout_ocl() << "Let's count with LOGGER_ROSOUT_OCL! " << std::hex << std::setfill('0');
	  for(int i = 0; i < 16; i++) log_rosout_ocl(WARN) << std::setw(2) << i << " ";
	  log_rosout_ocl() << Logger::endl << "New line" << Logger::endl;
  }
  log_rosout_ocl(ERROR) << "AAA! Its Ursa Major!!!" << RTT::Logger::endl;
  log_rosout_ocl(CRIT) << "Show must go on!" << RTT::Logger::endl;

  log_rosout_ocl(FATAL) << "Well, I'm not so great..." << Logger::discard;
  log_rosout_ocl(FATAL) << "Well, I'm not so great...";
  
  log_rosout_ocl(INFO) << "Come and witness amazing log of GREAT AND POWERFUL LOGGER_ROSOUT_OCL!" << endlog();
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
