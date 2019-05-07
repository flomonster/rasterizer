#pragma once

#include <assimp/scene.h>
#include <vector>

#include "color.hh"
#include "face.hh"

extern std::vector<aiLight *> lights;

struct Shader {
    bool face(const Face &face, const aiMaterial &);
    void vertex(Face &face, const aiMatrix4x4 &view_matrix);
    Color fragment(const Face &face, aiVector3D &point) const;

   private:
    const aiMaterial *material_;
};
