/*
    Copyright 2016 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#include <Game.h>

void setup_logging(const std::string& gameName) {
    class FormatterCustom : public logog::FormatterGCC {
        virtual TOPIC_FLAGS GetTopicFlags(const logog::Topic& topic) {
            return (logog::Formatter::GetTopicFlags(topic) &
                    ~(TOPIC_FILE_NAME_FLAG | TOPIC_LINE_NUMBER_FLAG));
        }
    };

    LOGOG_INITIALIZE();
    const std::__cxx11::string logFilename = gameName + ".log";
    // remove existing log file
    unlink(logFilename.c_str());
    // create the log's  file sink
    logog::LogFile outFile(logFilename.c_str());
    // create the log's console sink
    logog::Cout out;
    // create custom format
    FormatterCustom customFormat;
    customFormat.SetShowTimeOfDay(true);
    // use custom format
    out.SetFormatter(customFormat);
    outFile.SetFormatter(customFormat);
}

void dump_sys_info(const std::string& gameName) {
    const uint32_t majorVersion = 0;
    const uint32_t minorVersion = 4;
    const uint32_t revision = 1;

    INFO("%s %d.%d.%d %s %s", gameName.c_str(), majorVersion, minorVersion, revision, __DATE__, __TIME__);

    INFO("SFML %d.%d", SFML_VERSION_MAJOR, SFML_VERSION_MINOR);

    // what compiler are we using? just because
#ifdef __MINGW32__
    #ifdef __MINGW64__
    INFO("MinGW-w64 %d.%d", __MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR);
#else
    INFO("MinGW %d.%d", __MINGW32_MAJOR_VERSION, __MINGW32_MINOR_VERSION);
#endif
#endif
#ifdef __clang__
    INFO("CLang %d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#endif
#ifdef __GNUG__
    INFO("GCC %s", __VERSION__);
#endif
#ifdef MSC_VER
    INFO("Visual C++ %s", _MCS_VER);
#endif
}

int main(int argc, char* argv[]) {
    std::string gameName = "SpaceFight";

    setup_logging(gameName);

    dump_sys_info(gameName);

    Game& game = Game::getGame(gameName);
    if (game.ready()) {
        INFO("Ready to start!");
        game.run();
        return EXIT_SUCCESS;
    } else {
        ERR("Could not initialize Game, quitting.");
        return EXIT_FAILURE;
    }
}
