#include <rtt/plugin/ServicePlugin.hpp>
#include <rtt/Service.hpp>
#include <rtt/Logger.hpp>

#include <log4cpp/Category.hh>
#include <orocos/ocl/Category.hpp>
#include <log4cpp/PropertyConfigurator.hh>

#include <sweetie_bot_logger/logger.hpp>
#include <sweetie_bot_logger/RosAppender.hpp>

using namespace RTT;

class Log4cppService : public RTT::Service
{
protected:
	// Properties
	std::string default_category;

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
		this->addOperation("setPriority", &Log4cppService::setPriority, this)
			.doc("Set category priority. Use FATAL, ALERT, CRIT, ERROR, WARN, NOTICE, INFO, DEBUG, NOTSET value levels.")
			.arg("category_name", "Category name")
			.arg("priority", "log4cpp priority.");
		this->addOperation("getPriority", &Log4cppService::getPriority, this)
			.doc("Return category priority.")
			.arg("category_name", "Category name");
		this->addOperation("categoryFromComponentName", &Log4cppService::categoryFromComponentName, this)
			.doc("Construct logging category from component name according to sweetie_bot naming conventions using default_root_category as prefix.")
			.arg("component_name", "Component name, slashes are replaced by dots.");
		this->addOperation("getDefaultCategory", &Log4cppService::getDefaultCategory, this)
			.doc("Retern default_root_category property or default_category if it is equal to\"NOTSET\".")
			.arg("default_category", "Default category name if default_root_category property is not set.");

		this->addProperty("default_root_category", default_category)
			.doc("Default root logging category. Use 'logger::getDefaultCategory' to get it from C++ code. 'NOTSET' value is ignored. ")
			.set("NOTSET");
	}

	string getDefaultCategory(const string& _default_category) 
	{
		if (this->default_category == "NOTSET") return _default_category;
		else return this->default_category;
	}

	string categoryFromComponentName(const string& component_name)
	{
		return sweetie_bot::logger::categoryFromComponentName(component_name, getDefaultCategory(""));
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

	bool setPriority(const std::string& category_name, const std::string& level) 
	{
		RTT::Logger::In in("Log4cppService");
		log4cpp::Category * category = log4cpp::Category::exists(category_name);
		if (!category) {
			RTT::log(Error) << "Category '" << category_name << "' does not exist." << RTT::endlog();
			return false;
		}
		try {
			category->setPriority(log4cpp::Priority::getPriorityValue(level));
		}
		catch (std::invalid_argument& e) {
			RTT::log(Error) << "setPriority: unknown priority level: " << e.what() << endlog();
			return false;
		}
		return true;
	}

	std::string getPriority(const std::string& category_name) 
	{
		RTT::Logger::In in("Log4cppService");
		log4cpp::Category * category = log4cpp::Category::exists(category_name);
		if (!category) {
			RTT::log(Error) << "Category '" << category_name << "' does not exist." << RTT::endlog();
			return "";
		}
		return log4cpp::Priority::getPriorityName(category->getPriority());
	}

	void closeRosAppenders() 
	{
		//RTT::Logger::In in("Log4cppService");

		//RTT::log(Info) << "Close rosout connection ports." << RTT::endlog();
		// Close port before removing appender from all Categories.
		// Otherwise RTT::logger may try to log using appender during destruction. 
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
