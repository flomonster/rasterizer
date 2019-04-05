#include "shader.hh"
#include "project.hh"

bool Shader::face(const Face &face, const aiMesh &mesh) {
    return true;
}

void Shader::vertex(Face &face, const aiMatrix4x4 &view_matrix) {
    for (auto &vertex : face)
        vertex = multProject(view_matrix, vertex);
}

Color Shader::fragment(aiVector3D &point) const {
    return {1, 1, 1};
}
