/*
    Copyright 2014 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#define ELPP_NO_DEFAULT_LOG_FILE
#include <easylogging++.h>

#include <Game.h>

INITIALIZE_EASYLOGGINGPP

// version info
const unsigned int majorVersion = 0;
const unsigned int minorVersion = 2;
const unsigned int revision = 0;
// 16:9 window that fits inside a 1366x768 screen on most systems
const int width = 1200;
const int height = 675;
// our name
const std::string gameName = "SpaceFight";

int main(int argc, char *argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    // name the logFile after the game plus ".log"
    std::string logFilename = gameName;
    logFilename.append(".log");
    // remove existing log file, easylogging++ doesn't currently support non-append logs
    unlink(logFilename.c_str());
    // configure easylogging++
    el::Configurations logConf;
    // reset config
    logConf.setToDefault();
    // turn on logging
    logConf.setGlobally(el::ConfigurationType::Enabled, "true");
    // log to file named logFilename
    logConf.setGlobally(el::ConfigurationType::ToFile, "true");
    logConf.setGlobally(el::ConfigurationType::Filename, logFilename);
    // also log to console
    logConf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    // allow 3 digits for milliseconds when logging times
    logConf.setGlobally(el::ConfigurationType::MillisecondsWidth, "3");
    // use format of "date time log-level message"
    logConf.setGlobally(el::ConfigurationType::Format, "%datetime{%H:%m:%s.%g} %level %msg");
    // add the file and function location in debug logs
    logConf.set(el::Level::Debug, el::ConfigurationType::Format, "%datetime{%H:%m:%s.%g} %level %loc %func %msg");
    // use this config in all loggers
    el::Loggers::reconfigureAllLoggers(logConf);

    // log some system info
    LOG(INFO) << gameName << " " << majorVersion << "." << minorVersion << "." << revision;
    LOG(INFO) << "Built on " << __DATE__ << " at " << __TIME__;
#ifdef __MINGW32__
#ifdef __MINGW64__
    LOG(INFO) << "MinGW-w64 " << __MINGW64_VERSION_MAJOR << "." << __MINGW64_VERSION_MINOR;
#else
    LOG(INFO) << "MinGW " << __MINGW32_MAJOR_VERSION << "." << __MINGW32_MINOR_VERSION;
#endif
#endif
#ifdef __clang__
    LOG(INFO) << "CLang " << __clang_major << "." << __clang_minor << "." << __clang_patchlevel;
#endif
#ifdef __GNUG__
    LOG(INFO) << "GCC " << __VERSION__;
#endif
#ifdef MSC_VER
    LOG(INFO) << "Visual C++ " << _MCS_VER;
#endif
    LOG(INFO) << "SFML " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR;

    // get the Game singleton
    Game *game = Game::getGame();
    // set up the Game with out chosen window size
    game->init(gameName, width, height);
    // start the Game
    game->run();
    // once the Game stops running, we're done.
    return EXIT_SUCCESS;
}
