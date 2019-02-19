#pragma once


#include "defs.hh"


struct Color
{
  flt r;
  flt g;
  flt b;

  Color operator*(const Color& b) const;
  Color operator*(flt m) const;

  Color operator+(const Color& b) const;
};
