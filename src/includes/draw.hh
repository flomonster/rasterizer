#pragma once

#include <assimp/scene.h>  // Output data structure
#include <vector>

#include "color.hh"
#include "ppm.hh"

using Face = std::vector<aiVector3D>;

namespace draw {

void line(const aiVector3D& a, const aiVector3D& b, Image& img,
          const Color& col);

void draw(const std::vector<Face>& vertices, Image& img);
void triangle(const Face& face, Image& img, const Color& col);

}  // namespace draw
