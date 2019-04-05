#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <assimp/Importer.hpp>   // C++ importer interface

#include "shader.hh"
#include "draw.hh"
#include "ppm.hh"

Assimp::Importer importer;
const aiScene* scene;

std::unordered_map<std::string, aiMatrix4x4> object_transforms;

void import_scene(const std::string& pFile) {
    // Create an instance of the Importer class
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    scene = importer.ReadFile(
        pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                   aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene)
        throw std::runtime_error(importer.GetErrorString());
}

std::ostream& operator<<(std::ostream& out, const aiVector3D& vec) {
    return out << "<Vec3 " << vec.x << ", " << vec.y << ", " << vec.z << ">";
}

std::ostream& operator<<(std::ostream& out, const aiMatrix4x4& mat) {
    out << "<Mat4 ";
    for (int i = 0; i < 4; i++) {
        if (i)
            out << "      ";
        out << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << ", "
            << mat[i][3] << (i < 3 ? "\n" : ">");
    }
    return out;
}

static void flatten_node(std::vector<std::pair<Face, Shader>>& res,
                         aiMatrix4x4 transform,
                         const struct aiNode* node) {
    transform *= node->mTransformation;

    auto object_name = std::string{node->mName.data, node->mName.length};
    object_transforms.emplace(std::move(object_name), transform);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            auto face = Face();
            auto num_vertices = mesh->mFaces[j].mNumIndices;
            assert(num_vertices == 3);
            for (unsigned int k = 0; k < num_vertices; k++) {
                auto vertice = mesh->mVertices[mesh->mFaces[j].mIndices[k]];
                vertice *= transform;
                face[k] = vertice;
            }

            Shader face_shader;
            if (!face_shader.face(face, *mesh))
                continue;
            res.emplace_back(face, face_shader);
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        flatten_node(res, transform, node->mChildren[i]);
    }
}

aiCamera get_camera(const aiScene* scene) {
    assert(scene->HasCameras());
    auto camera = *scene->mCameras[0];
    auto camera_name = std::string{camera.mName.data, camera.mName.length};
    const auto& transform = object_transforms[camera_name];

    aiMatrix4x4 rotation;
    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 3; j++)
            rotation[i][j] = transform[i][j];

    aiMatrix4x4 translation;
    for (size_t i = 0; i < 3; i++)
        translation[i][3] = transform[i][3];

    camera.mPosition *= translation;
    camera.mLookAt *= rotation;
    camera.mUp *= rotation;
    return camera;
}

aiMatrix4x4 GetProjectionMatrix(const aiCamera& camera) {
    const float far = camera.mClipPlaneFar;
    const float near = camera.mClipPlaneNear;
    const auto xscale = 1.f / tan(camera.mHorizontalFOV/2);
    const auto yscale = xscale * camera.mAspect;

    auto Q = -far / (far - near);
    return  aiMatrix4x4(
        xscale, 0.0f, 0.0f, 0.0f,
        0.0f, yscale, 0.0f, 0.0f,
        0.0f, 0.0f, Q, Q * near, // this last coef is broken according to blender, needs more attention
        0.0f, 0.0f, -1.0f, 0.0f);
}

aiMatrix4x4 lookat(const aiVector3D& lookat, const aiVector3D& center,
                   const aiVector3D& up) {
    auto z = aiVector3D(lookat).Normalize();
    auto x = (up ^ lookat).Normalize();
    auto y = (x ^ z).Normalize();
    aiMatrix4x4 Minv{};
    aiMatrix4x4 Tr{};
    for (int i = 0; i < 3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }
    return Minv * Tr;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT_SCENE OUTPUT_PPM"
                  << std::endl;
        return 1;
    }

    const std::string input_scene{argv[1]};
    const char* output_path = argv[2];

    import_scene(input_scene);

    auto open_mode = (std::fstream::in | std::fstream::out |
                      std::fstream::binary | std::fstream::trunc);
    auto out = std::fstream(output_path, open_mode);
    if (!out.is_open()) {
        std::cerr << "failed to open: " << output_path << std::endl;
        return 1;
    }

    size_t width = 1920;
    size_t height = 1080;
    Image resulting_image{width, height};
    std::vector<std::pair<Face, Shader>> vertices;
    flatten_node(vertices, aiMatrix4x4{}, scene->mRootNode);

    auto camera = get_camera(scene);
    auto proj_matrix = (
        GetProjectionMatrix(camera) *
        lookat(camera.mLookAt, camera.mPosition, camera.mUp)
    );

    for (auto& [face, shader] : vertices)
        shader.vertex(face, proj_matrix);

    // Draw the scene
    draw::draw(vertices, resulting_image);

    // Save the rendered scene
    image_render_ppm(resulting_image, out);
}
