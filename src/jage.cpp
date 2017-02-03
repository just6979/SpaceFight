/*
 * Justin's Awesome Game Engine
 * or *
 * Just Another Game Engine
 *
 * Copyright 2016 Justin White
 *
 * See included LICENSE file for distribution and reuse.
*/

#include <cstdio>

#ifdef DO_DEBUG
#define LOGOG_LEVEL LOGOG_LEVEL_ALL
#else
#define LOGOG_LEVEL LOGOG_LEVEL_INFO
#endif
#include <logog/logog.hpp>

#include <Engine.hpp>

// keep logog vars in file scope, we manage them completely here
static logog::LogFile* outFile;
static logog::Cout* out;
static logog::Formatter* formatter;

void logging_setup(const std::string& gameName) {
    LOGOG_INITIALIZE();

    // create custom format
    class FormatterCustom : public logog::FormatterGCC {
        virtual TOPIC_FLAGS GetTopicFlags(const logog::Topic& topic) {
            return (logog::Formatter::GetTopicFlags(topic) &
                    ~(TOPIC_FILE_NAME_FLAG | TOPIC_LINE_NUMBER_FLAG));
        }
    };

    const std::string logFilename = gameName + '/' + gameName + ".log";

    // remove existing log file
    std::remove(logFilename.c_str());

    outFile = new logog::LogFile(logFilename.c_str());
    out = new logog::Cout;
    formatter = new FormatterCustom;
    // use custom format
    formatter->SetShowTimeOfDay(true);
    outFile->SetFormatter(*formatter);
    out->SetFormatter(*formatter);

    INFO("Logging system initialized.");
}

void logging_shutdown() {
    INFO("Logging system shutting down.");

    delete(outFile);
    delete(out);
    delete(formatter);

    LOGOG_SHUTDOWN();
}

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

    logging_setup(gameName);

    INFO("Start");
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

    logging_shutdown();
    return EXIT_SUCCESS;
}
