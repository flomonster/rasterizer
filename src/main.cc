#include <fstream>
#include <iostream>

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <assimp/Importer.hpp>   // C++ importer interface

#include <initializer_list>
#include <cassert>
#include <unordered_map>
#include <vector>
#include "draw.hh"
#include "ppm.hh"
#include <algorithm>

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

static void flatten_node(std::vector<Face>& res, aiMatrix4x4 transform,
                         const struct aiNode* node) {
    transform *= node->mTransformation;

    auto object_name = std::string{node->mName.data, node->mName.length};
    object_transforms.emplace(std::move(object_name), transform);

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            auto face_vec = Face();
            face_vec.reserve(3);
            for (unsigned int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
                auto vertice = mesh->mVertices[mesh->mFaces[j].mIndices[k]];
                std::cout << vertice.x << ' ' << vertice.y << ' ' << vertice.z
                          << '\n';
                vertice *= transform;
                face_vec.push_back(vertice);
                std::cout << vertice.x << ' ' << vertice.y << ' ' << vertice.z
                          << '\n';
            }
            assert(face_vec.size() == 3);
            res.push_back(std::move(face_vec));
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
    std::cout << "> transform is: " << std::endl << transform << std::endl;

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
    auto res =  aiMatrix4x4(
        xscale, 0.0f, 0.0f, 0.0f,
        0.0f, yscale, 0.0f, 0.0f,
        0.0f, 0.0f, Q, Q * near, // this last coef is broken according to blender, needs more attention
        0.0f, 0.0f, -1.0f, 0.0f);

    std::cout << "Camera matrix: " << std::endl;
    std::cout << res;
    return res;
}

template<class TReal>
aiVector3t<TReal> multProject (const aiMatrix4x4t<TReal>& pMatrix,
                               const aiVector3t<TReal>& pVector) {
    aiVector3t<TReal> res;
    // TODO: there was a 1 + right before d1 here. is it useful ?
    auto w = (pMatrix.d1 * pVector.x + pMatrix.d2 * pVector.y + pMatrix.d3 * pVector.z + pMatrix.d4);
    res.x = (pMatrix.a1 * pVector.x + pMatrix.a2 * pVector.y + pMatrix.a3 * pVector.z + pMatrix.a4) / w;
    res.y = (pMatrix.b1 * pVector.x + pMatrix.b2 * pVector.y + pMatrix.b3 * pVector.z + pMatrix.b4) / w;
    res.z = (pMatrix.c1 * pVector.x + pMatrix.c2 * pVector.y + pMatrix.c3 * pVector.z + pMatrix.c4) / w;
    return res;
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
    std::vector<Face> vertices;
    flatten_node(vertices, aiMatrix4x4{}, scene->mRootNode);

    auto camera = get_camera(scene);
    std::cout << camera.mPosition << std::endl;
    std::cout << camera.mLookAt << std::endl;
    std::cout << camera.mUp << std::endl;

    aiMatrix4x4 viewMatrix =
        lookat(camera.mLookAt, camera.mPosition, camera.mUp);

    auto proj_matrix = GetProjectionMatrix(camera);

    for (auto& vertex : vertices) {
        assert(vertex.size() == 3);
        for (auto& point : vertex) {
            point = viewMatrix * point;
            point = multProject(proj_matrix, point);
        }

        for (auto &[pa, pb] : {
                std::tie(vertex[0], vertex[1]),
                std::tie(vertex[1], vertex[2]),
                std::tie(vertex[2], vertex[0])
             }) {
            const size_t total_divs = 100;
            auto pvec = (pb - pa) * (1.0f / total_divs);
            for (size_t i = 0; i < total_divs; i++) {
                auto point = pa + pvec * (float)i;
                if (point.x < -1 || point.x > 1 ||
                    point.y < -1 || point.y > 1)
                    continue;

                uint32_t x = std::min(width - 1, (size_t)((point.x + 1) * 0.5 * width));
                uint32_t y = std::min(height - 1, (size_t)((1 - (point.y + 1) * 0.5) * height));
                resulting_image[y][x] = Color{1, 1, 1};
            }
        }
    }

    // // Draw the scene
    // draw::draw(vertices, resulting_image);

    // Save the rendered scene
    image_render_ppm(resulting_image, out);
}
