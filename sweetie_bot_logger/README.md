Great and Powerfull Logger
--------------------------

Great and Powerfull Logger is part of [Sweetie Bot](http://sweetiebot.net) project. 
It is meant as drop in replacment for RTT::Logger.

Native OROCOS logging system suffers a few drawbacks:

1. `RTT::Logger` does not support categories and individual log levels for components. Also it is not real-time.
2. `OCL::logging::Category` provides real-time logging service, but suffers from not very convinent 
    syntax for `<<` operator:
    
        log->getRTStream(log4cpp::ERROR) << 
    
    Moreover it creates temporary `ostringstream` any `getRTStream` call.
3. `log4cpp::Category` is not realtime, deployer does not support `log4cpp` out of box (recompiling is needed).
4. All logging systems lack ROS integration.

Great and Powerfull Loger is hear to fix thoese deficiencies! It has many **tricks** under its mantle:

1. `log4cpp::RosAppender` for `log4cpp` which forward messages to `\rosout` ROS topic. 
2. `RosAppenderService` which able to forward given `log4cpp` category to `\rosut` (use `addCategory` operation).
3. `OCL::logging::RosAppender` component which forward log from `OCL::logging` category to `\rosut` (use `OCL::logging::LoggerService` to setup connection).
4. Logger interface includes:
     * `LoggerLog4Cpp` logs directly to specified `log4cpp` category.
     * `LoggerOCL` logs to `OCL` category.
     * `LoggerRos` directry logs to `\rosout`, for log priority assigment may use `OCL` or `log4cpp` categories.
     * `LoggerRTT` uses RTT::Logger to display messages.
   Loggers do not rely on dynamic memory allocation and use `RTT::Loger` style syntax:
 
       using sweetie_bot::Logger;
       sweetie_bot::LoggerLog4Cpp log("org.orocos.rtt");

       log(Logger::INFO) << "I'm GREAT and POWERFULL LOGEER!" << Logger::endl;

   Great and Powerfull Logger generously creates categories for you (despite generosity is clearly not its element).
   Log is flushed by `Logger::endl` manipulator, `RTT::Logger` maipulators are also supported. See `LoggerTest` for example.

Usage
-----

### Deployer with `log4cpp` support

Use `RosAppenderService` to forward `RTT::Logger` messages to `rosout`, create categories with standart log4cpp file.
You may use all interfaces of Great and Powerfull Logger.

Test run:

	LOGGER_SCRIPTS=`rospack find sweetie_bot_logger`/scripts
    deployer --rtt-log4cpp-config-file $LOGGER_SCRIPTS/logging.log4cpp -s $LOGGER_SCRIPTS/test_logger.ops
	> test.trigger

All logger of `LoggerTest` component should be opertional. See `logger.log4cpp`, `logger.ops`, `log_ocl.cpf` for confuguratin example.


### Deployer without `log4cpp` support

Forward `RTT::Logger` to ROS is not possible, Great and Powerfull Logger able to work and in such coditions.
Create categories with `OCL::logging::LoggingService` and use `LoggerOCL` (with OCL appenders) or `LoggerRosout` (appender is not needed).

**Test run:**

	LOGGER_SCRIPTS=`rospack find sweetie_bot_logger`/scripts
    deployer -s $LOGGER_SCRIPTS/test_logger.ops
	> test.trigger


Two loggers using `log4cpp` categories should fail, `rosout` service also fails.

**Configuration** for OCL + Rosout logging infrastructure (DOES NOT WORKING!). 
In deployer run:

    scripting.loadPrograms(path_to_logger_scripts + "setup_logging.ops");
    setup_logging("logger_servie_config.cpf", "file_appender.log", false);


Known bugs and limitations
--------------------------

1. `LoggerRosut` uses `Mutex` to prevent concurrent por write.
2. If output port of `log4cpp:::RosAppender` is connected during desruction Ursa Major is being summomed (deadlock happens). 
Deadlock is caused by attemt to log via appender being destructed. Currently bug is amended with `assert`.
3. It seems rosout unable to process connection request fast enoughs. First bunch of messages from LoggerTest is missing on rosout. Just call sleep.



