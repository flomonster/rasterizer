#pragma once
#include <assimp/scene.h>
#include <math.h>
#include <ostream>

float dist(const aiVector3D& a, const aiVector3D& b);
std::ostream& operator<<(std::ostream& out, const aiVector3D& vec);
std::ostream& operator<<(std::ostream& out, const aiMatrix4x4& mat);
