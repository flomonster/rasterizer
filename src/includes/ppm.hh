#pragma once

#include <assimp/scene.h>  // Output data structure
#include <iostream>
#include <limits>
#include <vector>

#include "color.hh"

using Pixel = Color;

struct Image {
    Image(size_t w, size_t h)
        : w(w),
          h(h),
          data_(w * h),
          zbuffer_(w * h, std::numeric_limits<float>::min()),
          w2(w / 2),
          h2(h / 2) {
    }

    size_t w;
    size_t h;
    size_t w2;
    size_t h2;

    Pixel* operator[](size_t y) {
        return &data_[y * w];
    }

    const Pixel* operator[](size_t y) const {
        return &data_[y * w];
    }

    void draw(const aiVector3D& p, const Color& col) {
        int x = p.x;
        int y = p.y;
        if (x < 0 || x >= (int)w || y < 0 || y >= (int)h ||
            p.z < zbuffer_[y * w + x])
            return;
        zbuffer_[y * w + x] = p.z;
        data_[(h - y - 1) * w + x] = col;
    }

   private:
    std::vector<Pixel> data_;
    std::vector<float> zbuffer_;
};

std::ostream& image_render_ppm(const Image& img, std::ostream& out);
