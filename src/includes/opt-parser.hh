#pragma once

#include <boost/variant.hpp>
#include <vector>

namespace utils {
struct Options {
    /*! \brief Constructor given program argc and argv */
    Options(int argc, char* argv[]);

    /*! \brief In case of --pgn the given path to pgn */
    std::string input;
    /*! \brief In case of --perft the given path to perft */
    std::string output;
};
}  // namespace utils
