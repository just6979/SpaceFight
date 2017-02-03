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

#include <Engine.hpp>

int main(const int argc, const char** argv) {
    std::cout << "Start" << std::endl;;
    Engine theGame(argc, argv);
    if (theGame.ready()) {
        if (theGame.run()) {
        } else {
            std::cerr << "Error running Engine, quitting." << std::endl;;
            return EXIT_FAILURE;
        }
    } else {
        std::cerr << "Could not initialize Engine, quitting." << std::endl;;
        return EXIT_FAILURE;
    }
    std::cout << "Done" << std::endl;;

    return EXIT_SUCCESS;
}
