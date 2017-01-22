-- TEST: deploy 

-- common startup
depl = rtt.getTC():getPeer("Deployer")

-- import ROS plugins
require("rttros") -- find_rospack function

-- import logger helper functions
logger_scripts = rttros.find_rospack("sweetie_bot_logger") .. "/scripts/"
dofile(logger_scripts .. "logger.lua")

-- Init loglevels
logger.init_loglevels_log4cpp(logger_scripts .. "logging.log4cpp")
-- Alternatively:
-- logger.init_loglevels_cpf(logger_scripts .. "logging.cpf")

-- Redirect standart RTT::Logger output rosout
-- IMPORTANT! Works only for deployer with log4cpp support.
--
-- Bug! OROCOS does not call ~Service(), so port is connected during RosAppender destruction.
-- Log attempt from ConnectionManger causes deadlock. Currently bug is solved by adding
-- assert (port.disconnected) in  ~RosAppender().
-- 
logger.log4cpp:addRosAppender("org.orocos.rtt", 20) -- output Category for RTT::Logger, buffer for 20 messages

-- Load and start test component
depl:loadComponent("test", "sweetie_bot::LoggerTest")
test = depl:getPeer("test")
test.start()
