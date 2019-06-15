#include "opt-parser.hh"

#include <boost/program_options.hpp>
#include <iostream>

namespace opt = boost::program_options;

namespace utils {
Options::Options(int argc, char* argv[]) {
    opt::options_description desc("Allowed options");
    desc.add_options()("help,h", "show usage")(
        "input,in", opt::value<std::string>(&input), "path to the input file")(
        "output,out", opt::value<std::string>(&output),
        "path to output (ppm) file")("type,t", opt::value<std::string>(&type),
                                     "line|flat|phong");

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
        if (type == "line") {
        } else if (type == "falt") {
        } else if (type == "phong") {
        } else {
            std::cerr << "\"" << type << "\" isn't a valid type.\n";
            std::exit(1);
        }
    }
}
}  // namespace utils
