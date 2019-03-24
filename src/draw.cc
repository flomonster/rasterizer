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

aiVector3D barycentric(const Face& face, float x, float y) {
    aiVector3D u{aiVector3D{face[2][0] - face[0][0], face[1][0] - face[0][0],
                            face[0][0] - x} ^
                 aiVector3D{face[2][1] - face[0][1], face[1][1] - face[0][1],
                            face[0][1] - y}};
    if (std::abs(u[2]) < 1)
        return {-1, 1, 1};
    return {1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
}

void triangle(const Face& face, Image& img, const Color& col) {
    aiVector3D boxmin{img.w / 2.f - 1, img.h / 2.f - 1, 0};
    aiVector3D boxmax{img.w / -2.f, img.h / -2.f, 0};
    const aiVector3D clamp_max{img.w / 2.f - 1, img.h / 2.f - 1, 0};
    const aiVector3D clamp_min{img.w / -2.f, img.h / -2.f, 0};
    for (auto i = 0; i < 3; i++)
        for (auto j = 0; j < 2; j++) {
            boxmin[j] = std::max(clamp_min[j], std::min(boxmin[j], face[i][j]));
            boxmax[j] = std::min(clamp_max[j], std::max(boxmax[j], face[i][j]));
        }

    for (int x = boxmin.x; x < boxmax.x; x++)
        for (int y = boxmin.y; y < boxmax.y; y++) {
            auto bc_screen = barycentric(face, x, y);
            if (bc_screen.x >= 0 && bc_screen.y >= 0 && bc_screen.z >= 0) {
                // TODO: Improve the calculation of z
                img.draw({x, y, face[0].z}, col);
            }
        }
}

void draw(const std::vector<Face>& faces, Image& img) {
    for (const auto& f : faces) {
        Color c{(float)std::rand() / RAND_MAX, (float)std::rand() / RAND_MAX,
                (float)std::rand() / RAND_MAX};
        // line(f[0], f[1], img, c);
        // line(f[1], f[2], img, c);
        // line(f[2], f[0], img, c);
        triangle(f, img, c);
    }
}

}  // namespace draw
