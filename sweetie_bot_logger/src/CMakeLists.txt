#orocos_install_headers(RosAppender.hpp logger.hpp) 
#orocos_library(logger logger.cpp) # ...you may add multiple source files
#orocos_library(ros_appender RosAppender.cpp) # ...you may add multiple source files

orocos_component(ros_appender_component RosAppender-component.hpp RosAppender-component.cpp) # ...you may add multiple source files
target_link_libraries(ros_appender_component ros_appender)

orocos_component(logger_test_component LoggerTest-component.hpp LoggerTest-component.cpp) # ...you may add multiple source files
target_link_libraries(logger_test_component logger)

orocos_service(ros_appender_service RosAppender-plugin.cpp) # ...you may add multiple source files
target_link_libraries(ros_appender_service ros_appender)

