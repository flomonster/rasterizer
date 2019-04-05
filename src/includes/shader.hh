#pragma once

#include <assimp/scene.h>

#include "color.hh"
#include "face.hh"

struct Shader {
    bool face(const Face &face, const aiMesh &mesh);
    void vertex(Face &face, const aiMatrix4x4 &view_matrix);
    Color fragment(aiVector3D &point) const;
};
