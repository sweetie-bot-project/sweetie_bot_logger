#include <rtt/plugin/ServicePlugin.hpp>
#include <rtt/Service.hpp>
#include <rtt/Logger.hpp>

#include <log4cpp/Category.hh>
#include <orocos/ocl/Category.hpp>
#include <log4cpp/PropertyConfigurator.hh>

#include <sweetie_bot_logger/RosAppender.hpp>

using namespace RTT;

class Log4cppService : public RTT::Service
{
public:
	Log4cppService(RTT::TaskContext* tc) :
		Service("log4cpp", tc)
	{
		doc("Basic log4cpp functionality.");
		this->addOperation("configure", &Log4cppService::configure, this)
			.doc("Init log4cpp using configuration file.")
			.arg("filename", "log4cpp configuration file"); 	
		this->addOperation("shutdown", &Log4cppService::shutdown, this)
			.doc("Remove all appenders from all categories.");
		this->addOperation("addRosAppender", &Log4cppService::addRosAppender, this)
			.doc("Add \\rosout appender to the category.")
			.arg("category_name", "Category name")
			.arg("buffer_size", "Message buffer size");
		this->addOperation("removeRosAppender", &Log4cppService::removeRosAppender, this)
			.doc("Remove \\rosout appender from the category.")
			.arg("category_name", "Category name");
	}

	log4cpp::Appender * getRosAppender(log4cpp::Category * category) 
	{
		if (!category) return 0;
		log4cpp::AppenderSet appenders = category->getAllAppenders();	
		for(log4cpp::AppenderSet::iterator i = appenders.begin(); i != appenders.end(); i++) {
			if (dynamic_cast<log4cpp::RosAppender *>(*i)) return *i;
		}
		return 0;
	}

	bool addRosAppender(const std::string& category_name, int buffer_size) 
	{
		RTT::Logger::In in("Log4cppService");
		log4cpp::Category * category = log4cpp::Category::exists(category_name);
		if (!category) {
			RTT::log(Error) << "Category '" << category_name << "' does not exist." << RTT::endlog();
			return false;
		}
		if (dynamic_cast<OCL::logging::Category*>(category)) {
			RTT::log(Warning) << "Category '" << category_name << "' is actually OCL::logging::Category. Use RosAppender component to forward it to rosout." << RTT::endlog();
		}
		if (getRosAppender(category)) {
			RTT::log(Warning) << "RosAppender is already attached to category '" << category_name << "'." << RTT::endlog();
			return true;
		}
		try {
			category->addAppender(new log4cpp::RosAppender("rosout", buffer_size));
		}
		catch (std::exception& e) {
			RTT::log(Error) << "addAppender: " << e.what() << RTT::endlog();
			return false;
		}
		RTT::log(Info) << "RosAppender is attached to '" << category_name << "' category." << RTT::endlog();
		return true;
	}

	bool removeRosAppender(const std::string& category_name) 
	{
		RTT::Logger::In in("Log4cppService");
		log4cpp::Category * category = log4cpp::Category::exists(category_name);
		if (!category) {
			RTT::log(Error) << "Category '" << category_name << "' does not exist." << RTT::endlog();
			return false;
		}
		log4cpp::Appender * appender = getRosAppender(category);
		if (! appender ) {
			RTT::log(Warning) << "RosAppender does not present at category '" << category_name << "'." << RTT::endlog();
			return false;
		}
		appender->close();
		category->removeAppender(appender);
		RTT::log(Info) << "RosAppender is removed from '" << category_name << "' category." << RTT::endlog();
		return true;
	}

	bool configure(const std::string& filename) 
	{
		RTT::Logger::In in("Log4cppService");
		try {
			log4cpp::PropertyConfigurator::configure(filename);
		}
		catch (const log4cpp::ConfigureFailure& e) {
			RTT::log(Error) << "configure: " << e.what() << RTT::endlog();
			return false;
		}
		return true;
	}

	void closeRosAppenders() 
	{
		RTT::Logger::In in("Log4cppService");

		RTT::log(Info) << "Close rosout connection ports." << RTT::endlog();
		// Close port before removing appender from all Categories.
		// Otherwise RTT::logger may try to log using appender during destruction. Due to fix in RosAppender this code is unnecessary.
		std::vector<log4cpp::Category*> * categories = log4cpp::Category::getCurrentCategories();
		for(std::vector<log4cpp::Category*>::iterator c = categories->begin(); c != categories->end(); c++) {
			log4cpp::Appender * appender = getRosAppender(*c);
			if (appender && (*c)->ownsAppender(appender)) { 
				appender->close();
			}
		}
	}

	void shutdown() 
	{
		closeRosAppenders();
		log4cpp::Category::shutdown();
	}

	~Log4cppService() 
	{
		closeRosAppenders();
	}
};

ORO_SERVICE_NAMED_PLUGIN(Log4cppService, "log4cpp")
