#include <algorithm>

#include "draw.hh"
#include "opt-parser.hh"

namespace draw {

/**
 * Draw a line with a specific color
 */
void line(const aiVector3D& a, const aiVector3D& b, Image& img) {
    static const Color col{1.f, 1.f, 1.f};
    bool steep = false;
    auto p_min = aiVector3D{a.x * img.w2, -a.y * img.h2, 0};
    auto p_max = aiVector3D{b.x * img.w2, -b.y * img.h2, 0};
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
    int align_y = steep ? img.w2 : img.h2;
    int align_x = steep ? img.h2 : img.w2;
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

aiVector3D barycentric(const Face::triplet& face, float x, float y) {
    aiVector3D u{aiVector3D{face[2][0] - face[0][0], face[1][0] - face[0][0],
                            face[0][0] - x} ^
                 aiVector3D{face[2][1] - face[0][1], face[1][1] - face[0][1],
                            face[0][1] - y}};
    if (std::abs(u[2]) < 1)
        return {-1, 1, 1};
    return {1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
}

void triangle(const Face& f, Image& img, const Shader& shader) {
    auto face = f.vert;
    for (auto& v : face) {
        v.x = (v.x + 1) * img.w2;
        v.y = (v.y + 1) * img.h2;
    }
    aiVector3D boxmin{(float)img.w, (float)img.h, 0};
    aiVector3D boxmax{0, 0, 0};
    const aiVector3D clamp_max{(float)img.w, (float)img.h, 0};
    const aiVector3D clamp_min{0, 0, 0};
    for (auto i = 0; i < 3; i++)
        for (auto j = 0; j < 2; j++) {
            boxmin[j] = std::max(clamp_min[j], std::min(boxmin[j], face[i][j]));
            boxmax[j] = std::min(clamp_max[j], std::max(boxmax[j], face[i][j]));
        }

    // translate the bounding box to normalized -1 to 1 coordinates
    aiVector3D norm_boxmin{boxmin.x / img.w - 1, boxmin.y / img.h - 1, 0};
    aiVector3D norm_boxmax{boxmax.x / img.w - 1, boxmax.y / img.h - 1, 0};

    // multiplying by these floats translates an x or y of the bounding
    // box to normalized dimensions

    float intensity = std::rand() / (float) RAND_MAX;
    for (int x = boxmin.x; x < boxmax.x; x++)
        for (int y = boxmin.y; y < boxmax.y; y++) {
            auto bc_screen = barycentric(face, x, y);

            // not in the triangle
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                continue;

            auto depth = face[0].z;
            for (int i=0; i<3; i++)
              depth += face[i][2]*bc_screen[i];

            Color color{intensity, intensity, intensity} ;
            if (utils::options.type != "flat")
              color = shader.fragment(f, bc_screen);
            img.draw({(float)x, (float)y, depth}, color);
        }
}

void draw(const std::vector<std::pair<Face, Shader>>& faces, Image& img) {
    for (const auto& [face, shader] : faces) {
        if (utils::options.type == "line") {
            line(face.vert[0], face.vert[1], img);
            line(face.vert[1], face.vert[2], img);
            line(face.vert[2], face.vert[0], img);
        } else
            triangle(face, img, shader);
    }
}

}  // namespace draw
