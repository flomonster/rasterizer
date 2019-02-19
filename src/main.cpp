#include <iostream>

#include <assimp/cimport.h>      // Plain-C interface
#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

static bool DoTheImportThing(const char *pFile) {
    // Start the import on the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll t
    // probably to request more postprocessing than we do in this example.
    const aiScene *scene = aiImportFile(
        pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                   aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    // If the import failed, report it
    if (!scene) {
        std::cerr << "failed to import scene" << std::endl;
        return false;
    }

    // We're done. Release all resources associated with this import
    aiReleaseImport(scene);
    return true;
}

int main(int argc, char *argv) {
    std::cout << "Hello world" << std::endl;
}
