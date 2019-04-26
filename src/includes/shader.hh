#pragma once

#include <assimp/scene.h>
#include <vector>

#include "color.hh"
#include "face.hh"

struct Shader {
    bool face(const Face &face, const aiMesh &mesh);
    void vertex(Face &face, const aiMatrix4x4 &view_matrix,
                const std::vector<aiLight *> &lights,
                const std::vector<aiMaterial *> &materials);
    Color fragment(aiVector3D &point) const;

   private:
    Color color_{};
};
