#pragma once

#include <boost/variant.hpp>
#include <vector>

namespace utils {
struct Options {
    /*! \brief Constructor given program argc and argv */
    void parse(int argc, char* argv[]);

    /*! \brief The given path to the scene */
    std::string input;
    /*! \brief The output image path */
    std::string output;
    /*! \brief Will generate the image of a given type*/
    std::string type;
};

extern Options options;

}  // namespace utils
