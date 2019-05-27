#include "utils.hh"

float dist(const aiVector3D& a, const aiVector3D& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
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
