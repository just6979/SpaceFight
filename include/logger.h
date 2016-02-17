/*
Some shortcuts for spdlog:

-Add to your class definition
Logger logger;

-In constructor/initializer:
logger = getLogger();

-Now you can use the stream macros anywhere in your class, ex:
DEBUG << "This is a debug message."
*/
#pragma once

#include <spdlog/spdlog.h>

// save typing in each class we need to log from
typedef std::shared_ptr<spdlog::logger> Logger;
#define LOG_NAME "logger"
#define getLogger() spdlog::get(LOG_NAME)

// stream insertion macros
#define DEBUG logger->debug()
#define INFO logger->info()
#define ERR logger->error()

// quick stream macros
// expensive, but can be good in initializers or such
#define QDEBUG spdlog::get(LOG_NAME)->debug()
#define QINFO spdlog::get(LOG_NAME)->info()
#define QERR spdlog::get(LOG_NAME)->error()
