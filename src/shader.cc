#include <iostream>

#include "project.hh"
#include "shader.hh"
#include "utils.hh"

bool Shader::face(const Face &, const aiMesh &) {
    return true;
}

void Shader::vertex(Face &face, const aiMatrix4x4 &view_matrix,
                    const std::vector<aiLight *> &lights,
                    const std::vector<aiMaterial *> &) {
    // Handle ambiant lights
    auto n = (face[2] - face[0]) ^ (face[1] - face[0]);
    n.Normalize();
    float intensity = 0;
    for (const auto &l : lights) {
        auto dir = (l->mPosition - face[0]);
        dir.Normalize();
        intensity += n * dir;
    }
    intensity = std::clamp(intensity, 0.f, 1.f);
    color_ = Color{intensity, intensity, intensity};

    // Projection
    for (auto &vertex : face)
        vertex = multProject(view_matrix, vertex);
}

Color Shader::fragment(aiVector3D &) const {
    return color_;
}
