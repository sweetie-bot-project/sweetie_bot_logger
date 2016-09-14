#include <rtt/plugin/ServicePlugin.hpp>
#include <rtt/Service.hpp>
#include <rtt/Logger.hpp>

#include <log4cpp/Category.hh>
#include <orocos/ocl/Category.hpp>

#include <sweetie_bot_logger/RosAppender.hpp>

class RosAppenderService : public RTT::Service
{
public:
	RosAppenderService(RTT::TaskContext* tc) :
		Service("rosout", tc)
	{
		doc("Forward log4cpp log to rosout.");
		this->addOperation("addCategory", &RosAppenderService::addCategory, this, OwnThread)
			.doc("Add rosout appender to category.");
		this->addOperation("removeCategory", &RosAppenderService::removeCategory, this, OwnThread)
			.doc("Remove rosout appender.");
	}

	bool addCategory(const std::string category_name, int buffer_size) 
	{
		Logger::In in("RosAppenderService");
		log4cpp::Category * category = log4cpp::Category::exists(category_name);
		if (!category) {
			RTT::log(Error) << "Category '" << category_name << "' does not exist." << RTT::endlog();
			return false;
		}
		if (dynamic_cast<OCL::logging::Category*>(category)) {
			RTT::log(Error) << "Category '" << category_name << "' is actually OCL::logging::Category. Use RosAppender component to forward it to rosout." << RTT::endlog();
			return false;
		}
		if (category->getAppender("rosout")) {
			RTT::log(Warning) << "Appender rosout already present at category '" << category_name << "'." << RTT::endlog();
			return true;
		}
		try {
			category->addAppender(new log4cpp::RosAppender("rosout", buffer_size));
		}
		catch (std::exception& e) {
			RTT::log(Error) << "addAppender: " << e.what() << RTT::endlog();
			return false;
		}
		return true;
	}

	bool removeCategory(const std::string category_name) 
	{
		Logger::In in("RosAppenderService");
		log4cpp::Category * category = log4cpp::Category::exists(category_name);
		if (!category) {
			RTT::log(Error) << "Category '" << category_name << "' does not exist." << RTT::endlog();
			return false;
		}
		if (dynamic_cast<OCL::logging::Category*>(category)) {
			RTT::log(Error) << "Category '" << category_name << "' is actually OCL::logging::Category. Use RosAppender component to forward it to rosout." << RTT::endlog();
			return false;
		}
		log4cpp::Appender * appender = category->getAppender("rosout");
		if (! appender ) {
			RTT::log(Warning) << "Appender rosout does not present at category '" << category_name << "'." << RTT::endlog();
			return false;
		}
		appender->close();
		category->removeAppender(appender);
		return true;
	}

	~RosAppenderService() {
		// Close port before removing appender from all Categories.
		// Othervise RTT::logger may try to log using appender during destruction. This causes lock.
		std::vector<log4cpp::Category*> * categories = log4cpp::Category::getCurrentCategories();
		for(std::vector<log4cpp::Category*>::iterator c = categories->begin(); c != categories->end(); c++) {
			log4cpp::Appender * appender = (*c)->getAppender("rosout");
			if (appender) { 
				appender->close();
			}
		}
	}

};
	

ORO_SERVICE_NAMED_PLUGIN(RosAppenderService, "rosout")
