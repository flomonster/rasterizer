#pragma once

#include <iostream>
#include <vector>
#include "color.hh"

using Pixel = Color;

struct Image {
    Image(size_t w, size_t h) : w(w), h(h), data_(w * h) {
    }

    size_t w;
    size_t h;

    Pixel* operator[](size_t y) {
        return &data_[y * w];
    }

    const Pixel* operator[](size_t y) const {
        return &data_[y * w];
    }

   private:
    std::vector<Pixel> data_;
};

std::ostream& image_render_ppm(const Image& img, std::ostream& out);
