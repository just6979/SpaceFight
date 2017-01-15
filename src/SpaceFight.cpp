/*
    Copyright 2016 Justin White
    See included LICENSE file for details.
*/

#include <unistd.h>

#include <Game.h>

int main(const int argc, const char** argv) {
    std::string gameName = "SpaceFight";

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

    INFO("Logging system initialized");

    INFO("Getting Game instance");
    Game& game = Game::getGame(argc, argv, gameName);
    INFO("Checking Game readiness...");
    if (game.ready()) {
        INFO("Starting Game!");
        game.run();
        INFO("Game ended");
        return EXIT_SUCCESS;
    } else {
        ERR("Could not initialize Game, quitting.");
        return EXIT_FAILURE;
    }
}
