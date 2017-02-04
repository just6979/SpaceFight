/*
 * Justin's Awesome Game Engine
 * or *
 * Just Another Game Engine
 *
 * Copyright 2016 Justin White
 *
 * See included LICENSE file for distribution and reuse.
*/

#include <Engine.hpp>

int done(std::unique_ptr<Engine> theGame, int exitCode, const std::string& msg = "Done");

int main(const int argc, const char** argv) {
    std::cout << "Start" << std::endl;
    auto theGame = std::make_unique<Engine>(argc, argv);
    if (not theGame->ready()) {
        return done(std::move(theGame), EXIT_FAILURE, "Could not initialize Engine, quitting.");
    }
    if (not theGame->run()) {
        return done(std::move(theGame), EXIT_FAILURE, "Error running Engine, quitting.");
    }
    return done(std::move(theGame), EXIT_SUCCESS);
}

int done(std::unique_ptr<Engine> theGame, int exitCode, const std::string& msg) {
    // shutdown the Engine
    theGame.release();
    std::string fullMsg;
    if (exitCode == EXIT_SUCCESS) {
        fullMsg = "Success: " + msg;
    } else { //EXIT_FAILURE
        fullMsg = "Error: " + msg;
    }
    std::cout << fullMsg << std::endl;
    return exitCode;
}
