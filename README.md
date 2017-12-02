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

Great and Powerfull Loger is hear to fix thoese deficiencies! It has many **tricks** under its mantle.

1. `log4cpp::RosAppender` is `log4cpp` appender which forward messages to `\rosout` ROS topic. 
2. `RosAppenderService` (`rosout` service) is able to forward given `log4cpp` category to `\rosut` (use `addCategory` operation).
2. `Log4cppService` (`log4cpp` service) allows to initialize `log4cpp` using configuration file.
3. `OCL::logging::RosAppender` component  forwards log from `OCL::logging` category to `\rosut` (use `OCL::logging::LoggerService` to setup connection).
4. Logger interface includes:
     * `LoggerLog4Cpp` logs directly to specified `log4cpp` category.
     * `LoggerOCL` logs to `OCL` category.
     * `LoggerRos` directry logs to `\rosout`, for log priority assigment may use `OCL` or `log4cpp` categories.
     * `LoggerRTT` uses RTT::Logger to display messages.
   Loggers do not rely on dynamic memory allocation and use `RTT::Loger` style syntax:
 
       using sweetie_bot::Logger;
       sweetie_bot::LoggerLog4Cpp log("robot.component");

       log(Logger::INFO) << "I'm GREAT and POWERFULL LOGEER!" << Logger::endl;

   Great and Powerfull Logger creates unexisting categories (despite generosity is clearly not its element).
   But you should set log levels  via `log4cpp` configuration file or using `ocl::logging::LoggerService` component.
   By default log levels are `UNSET` so no messages are shown.

   Log is flushed by `Logger::endl` manipulator, `RTT::Logger` maipulators are also supported. See `LoggerTest` for example.

Usage
-----

Your should select the logger facility and declare `log4cpp` categories.

### Using `LoggerRTT` and `LoggerRos`

Your should configure loglevels using log4cpp configuration file or OCL LoggerService cpf file. 
See `scripts/logger.log4cpp` and `scripts/logger.cpf` for example.
This files contains log levels for hierarchical tree of categories see [log4cpp documentation](http://log4cpp.sourceforge.net) for more details.

Then there are three ways to provide deployer with this information:

1. Use `--rtt-log4cpp-config-file` option of `deployer` on startup. (Deployer must be compiled with log4cpp support).
2. Use `log4cpp` service to load `.log4cpp` file (see `scripts/test_logger.ops` and `scripts/test_logger_log4cpp.lua`).
3. Use `marshalling` service to load `.cpf` file in `OCL::LoggerService` component.

Basic deployment code looks like: 

    loadService("Deployer", "log4cpp");
    log4cpp.configure("logger.log4cpp");

or in lua (see `scripts/logger.lua` helper module and `scripts/test_logger_log4cpp.lua` for details):

    dofile(rttros.find_rospack("sweetie_bot_logger") .. "logger.lua")
    logger.init_loglevels_log4cpp("logging.log4cpp")


### Using `LoggerLog4cpp`

Use `log4cpp` configuration file to declare loglevels and appenders. See [log4cpp documentation](http://log4cpp.sourceforge.net) for details.
Example configuration may be found in `scripts/logger_log4cpp.log4cpp`. 

Also you may add `RosAppender` to any category to redirect messages to `/rosout`.

    log4cpp.addRosAppender("<category_name>", 20);


### Using `LoggerOCL`

Use OCL logging infrastructure deployment procedure as described in OROCOS documentation. 
See `scripts/test_logger_ocl.ops` as example.

### Redirecting `RTT::Logger` to `/rosout`

If deployer is compiled with log4cpp support you can redirect `RTT::Logger` category `org.orocos.rtt` to `\rosout`:

    log4cpp.addRosAppender("org.orocos.rtt", 20);

But during shutdown deadlock occurs due to logging attempt using appender being destructed.
Currently this bug is fixed by `assert`. Also you can call `log4cpp.shutdown` explicitly to prevent deadlock.

### Test scripts:

	LOGGER_SCRIPTS=`rospack find sweetie_bot_logger`/scripts

    deployer -s $LOGGER_SCRIPTS/test_logger.ops
    deployer -s $LOGGER_SCRIPTS/test_logger_log4cpp.ops
    rttlua -i $LOGGER_SCRIPTS/test_logger_log4cpp.lua
    deployer -s $LOGGER_SCRIPTS/test_logger_ocl.ops


Known bugs and limitations
--------------------------

1. `LoggerRosut` uses `Mutex` to prevent concurrent write.
2. If output port of `log4cpp:::RosAppender` is connected during object destruction Ursa Major is being summomed (deadlock happens). 
It is caused by attempt to log via appender being destructed. Currently bug is amended with `assert` but also `log4cpp.shutdown` just before exit.
3. It seems `\rosout` is unable to process connection requests fast enoughs. First bunch of messages from LoggerTest is missing on `\rosout`. Just call sleep.



