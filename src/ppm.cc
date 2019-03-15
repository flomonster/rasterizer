#include <iostream>
#include <iomanip>
#include "ppm.hh"
#include <cassert>

static inline void put_component(flt comp, std::ostream &out) {
    assert(comp >= 0 && comp <= 1);
    int curval = comp * 255;
    out << std::setfill(' ') << std::setw(4) << curval;
}

static void put_pixel(const Pixel& pix, std::ostream& out) {
    put_component(pix.r, out);
    put_component(pix.g, out);
    put_component(pix.b, out);
}

std::ostream& image_render_ppm(const Image& img, std::ostream& out) {
    out << "P3\n";
    out << img.w << " " << img.h << '\n';
    out << "255\n";

    for (size_t y = 0; y < img.h; y++) {
        for (size_t x = 0; x < img.w; x++)
            put_pixel(img[y][x], out);
        out << "\n";
    }

    return out;
}
