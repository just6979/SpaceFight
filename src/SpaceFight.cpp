/*
    Copyright 2014 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#include <logog.hpp>

#include <Game.h>

class FormatterCustom : public logog::FormatterGCC {
    virtual TOPIC_FLAGS GetTopicFlags(const logog::Topic& topic) {
        return (logog::Formatter::GetTopicFlags(topic) &
                ~(TOPIC_FILE_NAME_FLAG | TOPIC_LINE_NUMBER_FLAG));
    }
};

int main(int argc, char* argv[]) {
    // version info
    const uint32_t majorVersion = 0;
    const uint32_t minorVersion = 4;
    const uint32_t revision = 0;
    // our name
    const std::string gameName = "SpaceFight";

    LOGOG_INITIALIZE();
    logog::Cout out;
    logog::LogFile outFile("SpaceFight.log");
    FormatterCustom customFormat;
    customFormat.SetShowTimeOfDay(true);
    out.SetFormatter(customFormat);
    outFile.SetFormatter(customFormat);

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
    INFO("CLang %d.%d.%d", __clang_major, __clang_minor, __clang_patchlevel);
#endif
#ifdef __GNUG__
    INFO("GCC %s", __VERSION__);
#endif
#ifdef MSC_VER
    INFO("Visual C++ %s", _MCS_VER);
#endif
    INFO("Ready to start!");
    // get the Game singleton
    Game& game = Game::getGame();
    // set up the Game with out chosen window size
    if (!game.init(gameName)) {
        ERR("Could not initialize Game, quitting.");
        return EXIT_FAILURE;
    }
    // start the Game
    game.run();
    // game over!
    // we're done
    return EXIT_SUCCESS;
}
