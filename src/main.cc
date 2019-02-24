#include <fstream>
#include <iostream>

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <assimp/Importer.hpp>   // C++ importer interface

#include "ppm.hh"
#include "render.hh"

const aiScene& import_scene(const std::string& pFile) {
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(
        pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                   aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene)
        throw std::runtime_error(importer.GetErrorString());

    return *scene;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT_SCENE OUTPUT_PPM"
                  << std::endl;
        return 1;
    }

    const std::string input_scene{argv[1]};
    const char* output_path = argv[2];

    auto scene = import_scene(input_scene);

    auto open_mode = (std::fstream::in | std::fstream::out |
                      std::fstream::binary | std::fstream::trunc);
    auto out = std::fstream(output_path, open_mode);
    if (!out.is_open()) {
        std::cerr << "failed to open: " << output_path << std::endl;
        return 1;
    }

    Image img = scene_render();
    image_render_ppm(img, out);
}
