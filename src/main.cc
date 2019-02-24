#include <fstream>
#include <iostream>

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <assimp/Importer.hpp>   // C++ importer interface

#include "ppm.hh"

Assimp::Importer importer;
const aiScene* scene;
void import_scene(const std::string& pFile) {
    // Create an instance of the Importer class
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    scene = importer.ReadFile(
        pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                   aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene)
        throw std::runtime_error(importer.GetErrorString());
}

using Face = std::vector<aiVector3D>;

static void flatten_node(std::vector<Face>& res, aiMatrix4x4 transform,
                         const struct aiNode* node) {
    transform *= node->mTransformation;
    auto local_tr_mat = transform.Transpose();

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            auto face_vec = Face(3);
            for (unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
                auto vertice = mesh->mVertices[mesh->mFaces[j].mIndices[k]];
                std::cout << vertice.x << ' ' << vertice.y << ' ' << vertice.z
                          << '\n';
                vertice *= local_tr_mat;
                face_vec.push_back(vertice);
                std::cout << vertice.x << ' ' << vertice.y << ' ' << vertice.z
                          << '\n';
            }
            res.push_back(std::move(face_vec));
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        flatten_node(res, transform, node->mChildren[i]);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT_SCENE OUTPUT_PPM"
                  << std::endl;
        return 1;
    }

    const std::string input_scene{argv[1]};
    const char* output_path = argv[2];

    import_scene(input_scene);

    auto open_mode = (std::fstream::in | std::fstream::out |
                      std::fstream::binary | std::fstream::trunc);
    auto out = std::fstream(output_path, open_mode);
    if (!out.is_open()) {
        std::cerr << "failed to open: " << output_path << std::endl;
        return 1;
    }

    Image resulting_image{42, 42};

    std::vector<Face> res;
    flatten_node(res, aiMatrix4x4{}, scene->mRootNode);
    image_render_ppm(resulting_image, out);
}
