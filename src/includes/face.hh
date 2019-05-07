#pragma once

#include <assimp/scene.h>
#include <array>

struct Face {
    using triplet = std::array<aiVector3D, 3>;
    triplet vert;
    triplet norm;
};
