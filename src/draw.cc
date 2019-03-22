#include "draw.hh"

namespace draw {

void line(const aiVector3D& a, const aiVector3D& b, Image& img,
          const Color& col) {
    for (float t = 0.; t < 1.; t += .01) {
        int x = a.x + (b.x - a.x) * t;
        x += img.w / 2;
        int y = a.y + (b.y - a.y) * t;
        y += img.h / 2;
        img[y][x] = col;
    }
}

void draw(const std::vector<Face>& faces, Image& img) {
    Color c{1, 1, 1};
    for (const auto& f : faces) {
        line(f[0], f[1], img, c);
        line(f[1], f[2], img, c);
        line(f[2], f[0], img, c);
    }
}

}  // namespace draw
