/*
    Copyright 2014 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#define ELPP_NO_DEFAULT_LOG_FILE
#include <easylogging++.h>

#include <Game.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[]) {
    // version info
    const unsigned int majorVersion = 0;
    const unsigned int minorVersion = 3;
    const unsigned int revision = 0;
    // our name
    const std::string gameName = "SpaceFight";

    START_EASYLOGGINGPP(argc, argv);

    // name the logFile after the game plus ".log"
    std::string logFilename = gameName;
    logFilename.append(".log");
    // remove existing log file, easylogging++ doesn't currently support non-append logs
    unlink(logFilename.c_str());
    // configure easylogging++ from file
    el::Configurations logConf("easylogging.cfg");
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
    Game* game = Game::getGame();
    // set up the Game with out chosen window size
    if (!game->init(gameName)) {
        LOG(ERROR) << "Could not initialize Game, quitting.";
        return EXIT_FAILURE;
    }
    // start the Game
    game->run();
    // once the Game stops running, or if it doesn't initialize, we're done.
    return EXIT_SUCCESS;
}
