cmake_minimum_required(VERSION 2.6.3)
project(sweetie_bot_logger)

### ROS dependicies ###
# Find the RTT-ROS package (this transitively includes the Orocos CMake macros)
find_package(catkin REQUIRED COMPONENTS rtt_ros)
include_directories(include ${catkin_INCLUDE_DIRS})

### Orocos Dependencies ###
# Note that orocos_use_package() does not need to be called for any dependency
# listed in the package.xml file
orocos_use_package(ocl-logging REQUIRED)
orocos_use_package(orocos-log4cpp REQUIRED)
include_directories(SYSTEM ${USE_OROCOS_INCLUDE_DIRS})

### Orocos Targets ###

#Libraries: only .so modules from root CMakeLists are added to *.pc file.
orocos_library(logger src/logger.cpp) # ...you may add multiple source files
orocos_library(ros_appender src/RosAppender.cpp) # ...you may add multiple source files

# Rest of the configuration is in src/
add_subdirectory( src )

### Orocos Package Exports and Install Targets ###

# Generate install targets for header files
orocos_install_headers(DIRECTORY include/${PROJECT_NAME}/)

# Scripts installation
install(DIRECTORY scripts
	DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})

# Export package information (replaces catkin_package() macro)
orocos_generate_package(
	INCLUDE_DIRS include 
	DEPENDS orocos-log4cpp
	DEPENDS_TARGETS ocl-logging rtt_ros rtt_roscomm rtt_rosgraph_msgs
)

if (DEFINED _pkgconfig_error)
if (${_pkgconfig_error})
	message(FATAL_ERROR ${_pkgconfig_error})
endif()
else()
	message(FATAL_ERROR ${_pkgconfig_error})
endif()
