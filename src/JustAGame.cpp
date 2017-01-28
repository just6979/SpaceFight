/*
    Copyright 2016 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#ifdef DO_DEBUG
#define LOGOG_LEVEL LOGOG_LEVEL_ALL
#else
#define LOGOG_LEVEL LOGOG_LEVEL_INFO
#endif
#include <logog/logog.hpp>

#include <Engine.h>

/*
 * Simply initializes the Logog logging system and the Game singleton,
 * then transfers control to the Game.run() event loop.
 */
int main(const int argc, const char** argv) {
    std::string gameName;
    if (argc >= 1 && argv[1] != NULL) {
        gameName = std::string(argv[1]);
    } else {
        gameName = "game";
    }
    LOGOG_INITIALIZE();
    const std::string logFilename = gameName + ".log";
    // remove existing log file
    unlink(logFilename.c_str());
    // create the log's  file sink
    logog::LogFile outFile(logFilename.c_str());
    // create the log's console sink
    logog::Cout out;
    // create custom format
    class FormatterCustom : public logog::FormatterGCC {
        virtual TOPIC_FLAGS GetTopicFlags(const logog::Topic& topic) {
            return (logog::Formatter::GetTopicFlags(topic) &
                    ~(TOPIC_FILE_NAME_FLAG | TOPIC_LINE_NUMBER_FLAG));
        }
    };
    FormatterCustom customFormat;
    customFormat.SetShowTimeOfDay(true);
    // use custom format
    out.SetFormatter(customFormat);
    outFile.SetFormatter(customFormat);

    INFO("Logging system initialized.");

    INFO("Getting Game instance.");
    Game& game = Game::getGame(argc, argv, gameName);
    INFO("Checking Game readiness...");
    if (game.ready()) {
        INFO("Starting Game!");
        if (game.run()) {
            INFO("Game ended successfully!");
        } else {
            ERR("Error running game, quitting.");
            return EXIT_FAILURE;
        }
    } else {
        ERR("Could not initialize Game, quitting.");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
