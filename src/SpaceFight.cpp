/*
    Copyright 2014 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#define ELPP_NO_DEFAULT_LOG_FILE
#include <easylogging++.h>

#include <Game.h>

INITIALIZE_EASYLOGGINGPP

//system data
const unsigned int majorVersion = 0;
const unsigned int minorVersion = 2;
const unsigned int revision = 0;
const int width = 1200;
const int height = 675;
const std::string gameName = "SpaceFight";

int main(int argc, char *argv[]) {
    std::string logFilename = gameName;
    logFilename.append(".log");
    // remove existing log file, easylogging++ doesn't currently support non-append logs
    unlink(logFilename.c_str());

    START_EASYLOGGINGPP(argc, argv);

    el::Configurations logConf;
    logConf.setToDefault();
    logConf.setGlobally(el::ConfigurationType::Format, "%datetime{%H:%m:%s.%g} %level %msg");
    logConf.setGlobally(el::ConfigurationType::Filename, logFilename);
    logConf.setGlobally(el::ConfigurationType::Enabled, "true");
    logConf.setGlobally(el::ConfigurationType::ToFile, "true");
    logConf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    logConf.setGlobally(el::ConfigurationType::MillisecondsWidth, "3");
    logConf.set(el::Level::Debug, el::ConfigurationType::Format, "%datetime{%H:%m:%s.%g} %level %loc %func %msg");
    el::Loggers::reconfigureAllLoggers(logConf);

    LOG(INFO) << gameName << " v" << majorVersion << "." << minorVersion << "." << revision;
    LOG(INFO) << "Built " << __DATE__ << " " << __TIME__;
    LOG(INFO) << "GCC " << __VERSION__;
    LOG(INFO) << "SFML " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR;

    Game *game = Game::getGame();
    game->init(gameName, width, height);
    game->run();

    return EXIT_SUCCESS;
}
