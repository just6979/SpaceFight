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

static std::unique_ptr<Engine> theGame;

int success(const std::string& msg);
int fail(const std::string& msg);

int main(const int argc, const char** argv) {
    std::cout << "Start" << std::endl;
    theGame = std::make_unique<Engine>(argc, argv);
    if (not theGame->ready()) {
        return fail("Could not initialize Engine, quitting.");
    }
    if (not theGame->run()) {
        return fail("Error running Engine, quitting.");
    }
    return success("Done");
}

int success(const std::string& msg) {
    theGame.release();
    std::cout << msg << std::endl;
    return EXIT_SUCCESS;
}

int fail(const std::string& msg) {
    theGame.release();
    std::cerr << msg << std::endl;
    return EXIT_FAILURE;
}
