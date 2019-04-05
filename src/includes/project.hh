#pragma once

#include <assimp/scene.h>

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
