#include <iostream>
#include <fstream>

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <assimp/Importer.hpp>   // C++ importer interface

#include "render.hh"
#include "ppm.hh"


bool DoTheImportThing(const std::string& pFile) {
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(
        pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                   aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene) {
        std::cerr << importer.GetErrorString() << std::endl;
        return false;
    }

    // do stuff
    return true;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " OUTPUT_PPM" << std::endl;
        return 1;
    }

    const char *output_path = argv[1];
    DoTheImportThing("tests/cube.obj");

    auto open_mode = (std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    auto out = std::fstream(output_path, open_mode);
    if (!out.is_open()) {
        std::cerr << "failed to open: " << output_path << std::endl;
        return 1;
    }

    Image img = scene_render();
    image_render_ppm(img, out);
}
