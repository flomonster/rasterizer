#include "opt-parser.hh"

#include <boost/program_options.hpp>
#include <iostream>

namespace opt = boost::program_options;

namespace utils {

void Options::parse(int argc, char* argv[]) {
    opt::options_description desc("Allowed options");
    desc.add_options()("help,h", "show usage")(
        "input,in", opt::value<std::string>(&input), "path to the input file")(
        "output,out", opt::value<std::string>(&output),
        "path to output (ppm) file")("type,t", opt::value<std::string>(&type),
                                     "line | flat | lambert | phong (default)");

    opt::variables_map vm;
    try {
        opt::store(opt::parse_command_line(argc, argv, desc), vm);
        opt::notify(vm);
    } catch (...) {
        /* unknown options */
        std::cerr << desc << '\n';
        std::exit(1);
    }

    if (vm.count("help")) {
        /* found --help */
        std::cout << desc;
        std::exit(0);
    }

    if (!vm.count("input") || !vm.count("output")) {
        std::cerr << "Need an input and output file\n";
        std::exit(1);
    }
    if (vm.count("type")) {
        if (type != "lambert" && type != "line" && type != "falt" &&
            type != "phong") {
            std::cerr << "\"" << type << "\" isn't a valid type.\n";
            std::exit(1);
        }
    } else
        type = "phong";
}
}  // namespace utils
