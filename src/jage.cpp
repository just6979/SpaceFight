/*
 * Justin's Awesome Game Engine
 * or *
 * Just Another Game Engine
 *
 * Copyright 2016 Justin White
 *
 * See included LICENSE file for distribution and reuse.
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
 * Simply initializes the Logog logging system, and the Engine,
 * then transfers control to the Engine's event loop.
 */
int main(const int argc, const char** argv) {
    // check if a game directory is specified on the command line
    std::string gameName;
    if (argc > 1 and argv[1] != NULL) {
        gameName = std::string(argv[1]);
    } else {
        gameName = "game";
    }

    LOGOG_INITIALIZE();
    const std::string logFilename = gameName + '/' + gameName + ".log";
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

    Engine theGame(argc, argv, gameName);
    if (theGame.ready()) {
        if (theGame.run()) {
        } else {
            ERR("Error running Engine, quitting.");
            return EXIT_FAILURE;
        }
    } else {
        ERR("Could not initialize Engine, quitting.");
        return EXIT_FAILURE;
    }
    INFO("Done");
    return EXIT_SUCCESS;
}
