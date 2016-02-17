/*
    Copyright 2014 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#include <Logging.h>

#include <Game.h>

int main(int argc, char* argv[]) {
    // version info
    const uint32_t majorVersion = 0;
    const uint32_t minorVersion = 4;
    const uint32_t revision = 0;
    // our name
    const std::string gameName = "SpaceFight";

    // set up spdlog
    try {
        std::string logFilename = gameName;
        logFilename.append(".log");
        // remove existing log file, easylogging++ doesn't currently support non-append logs
        unlink(logFilename.c_str());
        // use async mode for _moar speed_!
        spdlog::set_async_mode(4096);
        // build the list of sinks: console and simple file
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
        sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_st>(logFilename, false));
        // make and register the logger using those sinks
        auto combined_logger = std::make_shared<spdlog::logger>(LOG_NAME, begin(sinks), end(sinks));
        spdlog::register_logger(combined_logger);
        spdlog::set_pattern("%H:%M:%S.%f %l %v");
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
    // bring the logger into scope, the same way other code will
    Logger logger = getLogger();

    INFO << gameName << " " << majorVersion << "." << minorVersion << "." << revision << " " << __DATE__ << " " <<
    __TIME__;
    INFO << "SFML " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR;
    // what compiler are we using? just because
#ifdef __MINGW32__
#ifdef __MINGW64__
    INFO << "MinGW-w64 " << __MINGW64_VERSION_MAJOR << "." << __MINGW64_VERSION_MINOR;
#else
    INFO << "MinGW " << __MINGW32_MAJOR_VERSION << "." << __MINGW32_MINOR_VERSION;
#endif
#endif
#ifdef __clang__
    INFO << "CLang " << __clang_major << "." << __clang_minor << "." << __clang_patchlevel;
#endif
#ifdef __GNUG__
    INFO << "GCC " << __VERSION__;
#endif
#ifdef MSC_VER
    INFO << "Visual C++ " << _MCS_VER;
#endif
    INFO << "Ready to start!";
    // get the Game singleton
    Game& game = Game::getGame();
    // set up the Game with out chosen window size
    if (!game.init(gameName)) {
        ERR << "Could not initialize Game, quitting.";
        return EXIT_FAILURE;
    }
    // start the Game
    game.run();
    // game over!
    // flush the logs, just in case
    logger.get()->flush();
    // we're done
    return EXIT_SUCCESS;
}
