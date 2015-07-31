//! @file easing.h
//! @brief Various interpolation for animations
//!
//! File contains different interpolation functions
//! including a number of different easing modes
//!
//! @author Eric D. Phillips
//! @bug No know bugs

#include <pebble.h>

//! List of different interpolation curves
typedef enum InterpolationCurve {
  CurveLinear,
  CurveQuadEaseIn,
  CurveQuadEaseOut,
  CurveQuadEaseInOut,
  CurveSinEaseIn,
  CurveSinEaseOut,
  CurveSinEaseInOut
} InterpolationCurve;


//! Interpolation for integer value
//! @param start The beginning value
//! @param end The ending value
//! @param percent The percent of the way into the animation
//! @param percent_max The maximum percent to end the animation at
//! @param curve The interpolation curve to use while calculating the new value
//! @return The interpolated value
int32_t interpolation_integer(int32_t start, int32_t end, int32_t percent, int32_t percent_max,
                              InterpolationCurve curve);


//! Interpolation for GPoint type
//! @param start The beginning point
//! @param end The ending point
//! @param percent The percent of the way into the animation
//! @param percent_max The maximum percent to end the animation at
//! @param curve The interpolation curve to use while calculating the new value
//! @return The interpolated point
GPoint interpolation_gpoint(GPoint start, GPoint end, int32_t percent, int32_t percent_max,
                            InterpolationCurve curve);
