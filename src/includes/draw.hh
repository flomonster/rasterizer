#pragma once

#include <assimp/scene.h>  // Output data structure

#include "color.hh"
#include "ppm.hh"
#include "shader.hh"
#include "face.hh"

namespace draw {

void line(const aiVector3D& a, const aiVector3D& b, Image& img,
          const Color& col);

void draw(const std::vector<std::pair<Face, Shader>>& vertices, Image& img);
void triangle(const Face& face, Image& img, const Color& col);

}  // namespace draw
