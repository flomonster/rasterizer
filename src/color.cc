#include "color.hh"

static inline flt flt_normalize(flt a) {
    if (a < 0.)
        return 0.;
    if (a > 1.)
        return 1.;
    return a;
}

Color Color::operator*(const Color& o) const {
    return Color{flt_normalize(r * o.r), flt_normalize(g * o.g),
                 flt_normalize(b * o.b)};
}
Color Color::operator*(flt m) const {
    return Color{flt_normalize(r * m), flt_normalize(g * m),
                 flt_normalize(b * m)};
}

Color Color::operator+(const Color& o) const {
    return Color{flt_normalize(r + o.r), flt_normalize(g + o.g),
                 flt_normalize(b + o.b)};
}

