#include <algorithm>

#include "draw.hh"

namespace draw {

aiVector3D barycentric(const Face& face, float x, float y) {
    aiVector3D u{aiVector3D{face[2][0] - face[0][0], face[1][0] - face[0][0],
                            face[0][0] - x} ^
                 aiVector3D{face[2][1] - face[0][1], face[1][1] - face[0][1],
                            face[0][1] - y}};
    if (std::abs(u[2]) < 1)
        return {-1, 1, 1};
    return {1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
}

void triangle(const Face& f, Image& img, const Shader& shader) {
    auto face = f;
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

    float box_width = boxmax.x - boxmin.x;
    float box_height = boxmax.y - boxmin.y;

    float norm_box_width = norm_boxmax.x - norm_boxmin.x;
    float norm_box_height = norm_boxmax.y - norm_boxmin.y;

    // multiplying by these floats translates an x or y of the bounding
    // box to normalized dimensions
    float box_w_ratio = norm_box_width / box_width;
    float box_h_ratio = norm_box_height / box_height;

    for (int x = boxmin.x; x < boxmax.x; x++)
        for (int y = boxmin.y; y < boxmax.y; y++) {
            auto bc_screen = barycentric(face, x, y);

            // not in the triangle
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                continue;

            // TODO: Improve the calculation of z
            auto depth = face[0].z;
            auto norm_point_x = norm_boxmin.x + x * box_w_ratio;
            auto norm_point_y = norm_boxmin.y + y * box_h_ratio;
            aiVector3D norm_point{norm_point_x, norm_point_y, depth};
            auto color = shader.fragment(norm_point);
            img.draw({(float)x, (float)y, depth}, color);
        }
}

void draw(const std::vector<std::pair<Face, Shader>>& faces, Image& img) {
    for (const auto& [face, shader] : faces)
        triangle(face, img, shader);
}

}  // namespace draw
