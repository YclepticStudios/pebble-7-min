//! @file easing.c
//! @brief Various interpolation for animations
//!
//! File contains different interpolation functions including a number
//! of different easing modes. Several convenience functions also exist
//! for interpolating different Pebble data types
//!
//! @author Eric D. Phillips
//! @bug No known bugs

#include <pebble.h>
#include "interpolation.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
//

// Linear interpolation
static int32_t prv_curve_linear(int32_t start, int32_t end, int32_t percent, int32_t percent_max) {
  return start + (end - start) * percent / percent_max;
}


// Quadratic interpolation in
static int32_t prv_curve_quad_ease_in(int32_t start, int32_t end, int32_t percent,
                                      int32_t percent_max) {
  int32_t t_percent = percent * 1000;
  t_percent /= percent_max;
  return (end - start) * t_percent * t_percent / 1000000 + start;
}


// Quadratic interpolation out
static int32_t prv_curve_quad_ease_out(int32_t start, int32_t end, int32_t percent,
                                       int32_t percent_max) {
  int32_t t_percent = percent * 1000;
  t_percent /= percent_max;
  return - (end - start) * t_percent * (t_percent - 2000) / 1000000 + start;
}


// Quadratic interpolation in out
static int32_t prv_curve_quad_ease_in_out(int32_t start, int32_t end, int32_t percent,
                                          int32_t percent_max) {
  int32_t t_percent = percent * 1000;
  t_percent /= percent_max / 2;
  if (t_percent < 1000) {
    return (end - start) / 2 * t_percent * t_percent / 1000000 + start;
  }
  t_percent -= 1000;
  return -(end - start) / 2 * (t_percent * (t_percent - 2000) - 1000000) / 1000000 + start;
}


// Sinusoidal interpolation in
static int32_t prv_curve_sin_ease_in(int32_t start, int32_t end, int32_t percent,
                                     int32_t percent_max) {
  return (-(end - start) * cos_lookup(TRIG_MAX_ANGLE * percent / percent_max / 4)) /
    TRIG_MAX_RATIO + (end - start) + start;
}


// Sinusoidal interpolation out
static int32_t prv_curve_sin_ease_out(int32_t start, int32_t end, int32_t percent,
                                      int32_t percent_max) {
  return ((end - start) * sin_lookup(TRIG_MAX_ANGLE * percent / percent_max / 4)) /
    TRIG_MAX_RATIO + start;
}


// Sinusoidal interpolation in out
static int32_t prv_curve_sin_ease_in_out(int32_t start, int32_t end, int32_t percent,
                                         int32_t percent_max) {
  return (-(end - start) * (cos_lookup(TRIG_MAX_ANGLE * percent / percent_max / 2) -
    TRIG_MAX_RATIO) / 2) / TRIG_MAX_RATIO + start;
}


// Array of interpolation function pointers
typedef int32_t (*InterpolationFunction)(int32_t, int32_t, int32_t, int32_t);
InterpolationFunction interpolation_functions[] = {
  prv_curve_linear,
  prv_curve_quad_ease_in,
  prv_curve_quad_ease_out,
  prv_curve_quad_ease_in_out,
  prv_curve_sin_ease_in,
  prv_curve_sin_ease_out,
  prv_curve_sin_ease_in_out
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// API Functions
//

// Interpolate an integer
int32_t interpolation_integer(int32_t start, int32_t end, int32_t percent, int32_t percent_max,
                              InterpolationCurve curve) {
  return interpolation_functions[curve](start, end, percent, percent_max);
}


// Interpolate a GPoint
GPoint interpolation_gpoint(GPoint start, GPoint end, int32_t percent, int32_t percent_max,
                            InterpolationCurve curve) {
  return GPoint(interpolation_integer(start.x, end.x, percent, percent_max, curve),
                interpolation_integer(start.y, end.y, percent, percent_max, curve));
}
