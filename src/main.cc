#include <fstream>
#include <iostream>

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <assimp/Importer.hpp>   // C++ importer interface

#include <cassert>
#include <unordered_map>
#include <vector>
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

using Face = std::vector<aiVector3D>;

std::ostream& operator<<(std::ostream& out, const aiVector3D& vec) {
    return out << "<Vec3 " << vec.x << ", " << vec.y << ", " << vec.z << ">";
}

std::ostream& operator<<(std::ostream& out, const aiMatrix4x4& mat) {
    out << "<Mat4 ";
    out << mat.a1 << ", ";
    out << mat.a2 << ", ";
    out << mat.a3 << ", ";
    out << mat.a4 << " | ";
    out << mat.b1 << ", ";
    out << mat.b2 << ", ";
    out << mat.b3 << ", ";
    out << mat.b4 << " | ";
    out << mat.c1 << ", ";
    out << mat.c2 << ", ";
    out << mat.c3 << ", ";
    out << mat.c4 << " | ";
    out << mat.d1 << ", ";
    out << mat.d2 << ", ";
    out << mat.d3 << ", ";
    out << mat.d4 << ">";
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
    std::cout << "> transform is: " << transform << std::endl;
    camera.mPosition *= transform;
    camera.mLookAt *= transform;
    camera.mUp *= transform;
    return camera;
}

// aiMatrix4x4 get_camera_transform(up, direction) {
//     auto xaxis = up ^ direction;
//     xaxis.Normalize();

//     auto yaxis = direction ^ xaxis;
//     yaxis.Normalize();

//     1.x = xaxis.x;
//     1.y = yaxis.x;
//     1.z = direction.x;

//     2.x = xaxis.y;
//     2.y = yaxis.y;
//     2.z = direction.y;

//     3.x = xaxis.z;
//     3.y = yaxis.z;
//     3.z = direction.z;
// }

aiMatrix4x4 GetProjectionMatrix(size_t width, size_t height,
                                const aiCamera& camera) {
    const float fFarPlane = camera.mClipPlaneFar;
    const float fNearPlane = camera.mClipPlaneNear;
    const auto fFOV = camera.mHorizontalFOV;
    const float s = 1.0f / tanf(fFOV * 0.5f);
    const float Q = fFarPlane / (fFarPlane - fNearPlane);

    const float fAspect = (float)width / (float)height;

    return aiMatrix4x4(s / fAspect, 0.0f, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f,
                       0.0f, -Q, -1.0f, 0.0f, 0.0f, -Q * fNearPlane, 0.0f);
}

aiMatrix4x4 lookat(const aiVector3D& lookat, const aiVector3D& center,
                   const aiVector3D& up) {
    auto z = (lookat - center).Normalize();
    // auto z = (-lookat).Normalize();
    auto x = (up ^ z).Normalize();
    auto y = (z ^ x).Normalize();
    aiMatrix4x4 Minv{};
    aiMatrix4x4 Tr{};

    Minv.a1 = x[0];
    Minv.a2 = x[1];
    Minv.a3 = x[2];
    Minv.b1 = y[0];
    Minv.b2 = y[1];
    Minv.b3 = y[2];
    Minv.c1 = z[0];
    Minv.c2 = z[1];
    Minv.c3 = z[2];
    Tr.a3 = -center[0];
    Tr.b3 = -center[1];
    Tr.c3 = -center[2];
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

    size_t width = 42;
    size_t height = 42;
    Image resulting_image{width, height};
    std::vector<Face> vertices;
    flatten_node(vertices, aiMatrix4x4{}, scene->mRootNode);
    for (size_t y = 0; y < 42; y++)
        for (size_t x = 0; x < 42; x++)
            resulting_image[y][x] =
                Color{(x + y) / 100.0, (43 + x - y) / 100.0, 0.0};

    image_render_ppm(resulting_image, out);

    std::cout << ">> Objects:" << std::endl;
    for (auto& e : object_transforms)
        std::cout << e.first << std::endl;

    auto camera = get_camera(scene);
    std::cout << camera.mPosition << std::endl;
    std::cout << camera.mLookAt << std::endl;
    std::cout << camera.mUp << std::endl;

    aiMatrix4x4 viewMatrix =
        lookat(camera.mLookAt, camera.mPosition, camera.mUp);

    auto proj_matrix = GetProjectionMatrix(width, height, camera);

    // for (auto& vertex : vertices) {
    //    assert(vertex.size() == 3);
    //    std::cout << "v";
    //    for (auto& point : vertex) {
    //        std::cout << "\t(" << point.x << ", " << point.y << ", " <<
    //        point.z
    //                  << ")";
    //    }
    //    std::cout << std::endl;
    //}

    for (auto& vertex : vertices) {
        assert(vertex.size() == 3);
        std::cout << "(";
        for (auto& point : vertex) {
            point = proj_matrix * viewMatrix * point;
            point.x /= point.z;
            point.y /= point.z;
            std::cout << "(" << point.x << ", " << point.y << "),";
        }
        std::cout << ")," << std::endl;
    }
}
