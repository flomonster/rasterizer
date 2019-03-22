#include <algorithm>

#include "draw.hh"

namespace draw {

/**
 * Draw a line with a specific color
 */
void line(const aiVector3D& a, const aiVector3D& b, Image& img,
          const Color& col) {
    bool steep = false;
    auto p_min = aiVector3D{a.x, a.y, 0};
    auto p_max = aiVector3D{b.x, b.y, 0};
    if (std::abs(a.x - b.x) < std::abs(a.y - b.y)) {
        std::swap(p_min.x, p_min.y);
        std::swap(p_max.x, p_max.y);
        steep = true;
    }
    if (p_min.x > p_max.x) {
        std::swap(p_min.x, p_max.x);
        std::swap(p_min.y, p_max.y);
    }
    float derror = std::abs((p_max.y - p_min.y) / (p_max.x - p_min.x));
    float error = 0;
    int align_y = steep ? img.w / 2 : img.h / 2;
    int align_x = steep ? img.h / 2 : img.w / 2;
    int y = p_min.y + align_y;
    for (int x = p_min.x + align_x; x <= p_max.x + align_x; x++) {
        if (steep)
            img[x][y] = col;
        else
            img[y][x] = col;

        error += derror;
        if (error > .5) {
            y += (p_max.y > p_min.y ? 1 : -1);
            error -= 1.;
        }
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
