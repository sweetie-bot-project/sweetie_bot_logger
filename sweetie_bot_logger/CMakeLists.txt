#
# The find_package macro for Orocos-RTT works best with
# cmake >= 2.6.3
#
cmake_minimum_required(VERSION 2.6.3)

#
# This creates a standard cmake project. You may extend this file with
# any cmake macro you see fit.
#
project(sweetie_bot_logger)

# Use 'source orocos_toolchain/env.sh' if the command below fails:
find_package(OROCOS-RTT QUIET)
if (NOT OROCOS-RTT_FOUND)
  message (FATAL_ERROR "\nCould not find Orocos. Please use the shell command\n 'source orocos_toolchain/env.sh' and then run cmake again.")
endif() 
# Defines the orocos_* cmake macros. See that file for additional
# documentation.
include( ${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake )

#Catkin support
find_package(catkin REQUIRED)
catkin_package(INCLUDE_DIRS include)
include_directories(include ${catkin_INCLUDE_DIRS})

#Add OROCOS libraries.
orocos_use_package(orocos-log4cpp)
orocos_use_package(ocl-logging)

#Install headers: OROCOS automatically place them in `orocos/package_name/` directory.
orocos_install_headers(include/sweetie_bot_logger/RosAppender.hpp include/sweetie_bot_logger/logger.hpp) 

#Libraries: only .so modules from root CMakeLists are added to *.pc file.
orocos_library(logger src/logger.cpp) # ...you may add multiple source files
orocos_library(ros_appender src/RosAppender.cpp) # ...you may add multiple source files

# Rest of the configuration is in src/
add_subdirectory( src )

#
# Generates and installs our package. Must be the last statement such
# that it can pick up all above settings.
#
orocos_generate_package()
