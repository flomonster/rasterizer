#include <iostream>

#include "opt-parser.hh"
#include "project.hh"
#include "shader.hh"
#include "utils.hh"

bool Shader::face(const Face &, const aiMaterial &mat) {
    material_ = &mat;
    return true;
}

void Shader::vertex(Face &face, const aiMatrix4x4 &view_matrix) {
    // Lambert shading
    if (utils::options.type == "lambert") {
        auto n = (face.vert[2] - face.vert[0]) ^ (face.vert[1] - face.vert[0]);
        n.Normalize();
        for (const auto &l : lights) {
            auto dir = (face.vert[0] - l.mPosition);
            dir.Normalize();
            intensity_ += n * dir;
        }
    }

    // Projection
    for (auto &vertex : face.vert)
        vertex = multProject(view_matrix, vertex);
}

Color Shader::fragment(const Face &face, aiVector3D &bc) const {
    float intensity = intensity_;
    // Phong shading
    if (utils::options.type == "phong")
        for (int i = 0; i < 3; ++i)
            for (const auto &l : lights) {
                auto dir = (l.mPosition - face.vert[i]);
                dir.Normalize();
                float angle_coeff = face.norm[i] * dir;
                intensity += angle_coeff * bc[i];
            }

    intensity = std::clamp(intensity, 0.f, 1.f);
    return Color{intensity, intensity, intensity};
}
