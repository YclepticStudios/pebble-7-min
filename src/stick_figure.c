// @file stick_figure.c
// @brief Code to draw and animate a stick figure
//
// File contains code necessary to draw a stick figure onto the graphics context
// as well as code necessary to animate the model between different poses.
// This is done by having end positions for each movement and interpolating between them
// using different easing functions. Each intermediate step is then drawn to give a much
// smoother animation using far fewer actual data frames.
// All data is stored in the code.
//
// @author Eric D. Phillips
// @bug No know bugs

#include <pebble.h>
#include "stick_figure.h"
#include "interpolation.h"

// skeleton containing all points for body
typedef struct SkeletonFrame {
  uint8_t             frame_count;    //< The total number of frames in this animation
  uint16_t            ani_duration;   //< Length of time to take to animate to this frame in ms
  uint16_t            pause_duration; //< Length of time to pause before starting next frame in ms
  InterpolationCurve  interp_curve;   //< Curve to use animating into this function
  GPoint              head;           //< Point for head position
  GPoint              chest;          //< Point for chest position
  GPoint              elbow_r;        //< Point for right elbow position
  GPoint              hand_r;         //< Point for right hand position
  GPoint              elbow_l;        //< Point for left elbow position
  GPoint              hand_l;         //< Point for left hand position
  GPoint              waist;          //< Point for waist position
  GPoint              knee_r;         //< Point for right knee position
  GPoint              foot_r;         //< Point for right foot position
  GPoint              knee_l;         //< Point for left knee position
  GPoint              foot_l;         //< Point for left foot position
} SkeletonFrame;


// stick figure structure declaration
typedef struct StickFigure {
  StickFigurePose   pose;           //< The StickFigurePose that it animates toward
  uint8_t           frame;          //< The frame within the pose that actually contains the points
  int64_t           frame_start;    //< The epoch in milliseconds when the frame animation started
  SkeletonFrame     old_skeleton;   //< The coordinates to animate away from
  SkeletonFrame     cur_skeleton;   //< The current coordinates of all body points
} StickFigure;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Pose Skeleton Data
//

// Rest
static const SkeletonFrame skeleton_data_rest[] = {
    {2, 400, 400, CurveSinEaseOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-7, 28}, {-10, 44}, {7, 28}, {10, 44}},
    {2, 400, 400, CurveSinEaseOut, {0, -31}, {0, -11}, {-12, -9}, {-17, 2}, {12, -9}, {17, 2}, {0, 14}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}}};
// Waiting for start
static const SkeletonFrame skeleton_data_waiting[] = {
    // wave
    {28, 400, 1400, CurveSinEaseOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 400, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 200, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 200, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 150, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    // pause
    {28, 400, 1000, CurveSinEaseInOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    // walking
    {28, 200, 0, CurveSinEaseIn, {0, -30}, {0, -10}, {-9, -1}, {-10, 13}, {5, 2}, {18, 6}, {0, 15}, {10, 27}, {16, 45}, {-4, 30}, {-17, 44}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {-3, 4}, {4, 13}, {-5, 5}, {-5, 17}, {0, 13}, {0, 31}, {-3, 49}, {3, 30}, {-7, 44}},
    {28, 200, 0, CurveLinear, {0, -30}, {0, -10}, {5, 2}, {18, 6}, {-9, -1}, {-10, 13}, {0, 15}, {-4, 30}, {-17, 44}, {10, 27}, {16, 45}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {-5, 5}, {-5, 17}, {-3, 4}, {4, 13}, {0, 13}, {3, 30}, {-7, 44}, {0, 31}, {-3, 49}},
    {28, 200, 0, CurveLinear, {0, -30}, {0, -10}, {-9, -1}, {-10, 13}, {5, 2}, {18, 6}, {0, 15}, {10, 27}, {16, 45}, {-4, 30}, {-17, 44}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {-3, 4}, {4, 13}, {-5, 5}, {-5, 17}, {0, 13}, {0, 31}, {-3, 49}, {3, 30}, {-7, 44}},
    {28, 200, 0, CurveLinear, {0, -30}, {0, -10}, {5, 2}, {18, 6}, {-9, -1}, {-10, 13}, {0, 15}, {-4, 30}, {-17, 44}, {10, 27}, {16, 45}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {-5, 5}, {-5, 17}, {-3, 4}, {4, 13}, {0, 13}, {3, 30}, {-7, 44}, {0, 31}, {-3, 49}},
    // wave
    {28, 400, 1400, CurveSinEaseOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 400, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 200, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 200, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -12}, {-27, -20}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {28, 150, 0, CurveSinEaseInOut, {0, -30}, {0, -10}, {-14, -13}, {-21, -25}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    // pause
    {28, 400, 1000, CurveSinEaseInOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    // walking
    {28, 200, 0, CurveSinEaseIn, {0, -30}, {0, -10}, {9, -1}, {10, 13}, {-5, 2}, {-18, 6}, {0, 15}, {-10, 27}, {-16, 45}, {4, 30}, {17, 44}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {3, 4}, {-4, 13}, {5, 5}, {5, 17}, {0, 13}, {-0, 31}, {3, 49}, {-3, 30}, {7, 44}},
    {28, 200, 0, CurveLinear, {0, -30}, {0, -10}, {-5, 2}, {-18, 6}, {9, -1}, {10, 13}, {0, 15}, {4, 30}, {17, 44}, {-10, 27}, {-16, 45}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {5, 5}, {5, 17}, {3, 4}, {-4, 13}, {0, 13}, {-3, 30}, {7, 44}, {-0, 31}, {3, 49}},
    {28, 200, 0, CurveLinear, {0, -30}, {0, -10}, {9, -1}, {10, 13}, {-5, 2}, {-18, 6}, {0, 15}, {-10, 27}, {-16, 45}, {4, 30}, {17, 44}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {3, 4}, {-4, 13}, {5, 5}, {5, 17}, {0, 13}, {-0, 31}, {3, 49}, {-3, 30}, {7, 44}},
    {28, 200, 0, CurveLinear, {0, -30}, {0, -10}, {-5, 2}, {-18, 6}, {9, -1}, {10, 13}, {0, 15}, {4, 30}, {17, 44}, {-10, 27}, {-16, 45}},
    {28, 200, 0, CurveLinear, {0, -33}, {0, -12}, {5, 5}, {5, 17}, {3, 4}, {-4, 13}, {0, 13}, {-3, 30}, {7, 44}, {-0, 31}, {3, 49}}};
// Jumping jacks
static const SkeletonFrame skeleton_data_jumping_jacks[] = {
    {4, 200, 150, CurveSinEaseIn, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {4, 200, 0, CurveSinEaseOut, {0, -35}, {0, -15}, {-12, -17}, {-24, -19}, {12, -17}, {24, -19}, {0, 10}, {-11, 19}, {-17, 32}, {11, 19}, {17, 32}},
    {4, 200, 150, CurveSinEaseIn, {0, -26}, {0, -6}, {-11, -7}, {-21, -16}, {11, -7}, {21, -16}, {0, 19}, {-10, 31}, {-20, 44}, {10, 31}, {20, 44}},
    {4, 200, 0, CurveSinEaseOut, {0, -35}, {0, -15}, {-12, -17}, {-24, -19}, {12, -17}, {24, -19}, {0, 10}, {-11, 19}, {-17, 32}, {11, 19}, {17, 32}}};
// Wall sit
static const SkeletonFrame skeleton_data_wall_sit[] = {
    {2, 400, 2000, CurveSinEaseInOut, {5, -20}, {0, 0}, {7, 10}, {17, 15}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}},
    {2, 400, 400, CurveSinEaseInOut, {7, -18}, {0, 0}, {12, 0}, {20, -9}, {7, 10}, {17, 15}, {0, 25}, {15, 26}, {16, 41}, {15, 26}, {16, 41}}};
// Push-ups
static const SkeletonFrame skeleton_data_pushups[] = {
    {2, 400, 400, CurveSinEaseInOut, {28, 12}, {6, 12}, {-2, 19}, {7, 22}, {-2, 19}, {7, 22}, {-9, 14}, {-32, 20}, {-47, 23}, {-32, 20}, {-47, 23}},
    {2, 400, 400, CurveSinEaseInOut, {25, -4}, {4, 0}, {5, 11}, {7, 22}, {5, 11}, {7, 22}, {-21, 7}, {-33, 16}, {-47, 23}, {-33, 16}, {-47, 23}}};
// Crunches
static const SkeletonFrame skeleton_data_crunches[] = {
    {2, 400, 400, CurveSinEaseInOut, {-26, 19}, {-4, 21}, {-14, 26}, {-23, 18}, {-14, 26}, {-23, 18}, {20, 21}, {21, 6}, {36, 6}, {21, 6}, {36, 6}},
    {2, 400, 400, CurveSinEaseInOut, {-17, 5}, {-4, 20}, {3, 11}, {-5, 2}, {3, 11}, {-5, 2}, {20, 21}, {17, 6}, {29, 1}, {17, 6}, {29, 1}}};
// Step-ups
static const SkeletonFrame skeleton_data_step_ups[] = {
    {4, 400, 400, CurveSinEaseInOut, {-14, -19}, {-14, 0}, {-13, 11}, {-9, 21}, {-13, 11}, {-9, 21}, {-14, 23}, {-13, 36}, {-14, 50}, {-13, 36}, {-14, 50}},
    {4, 400, 0, CurveSinEaseInOut, {5, -16}, {-2, 3}, {6, 9}, {16, 6}, {6, 9}, {16, 6}, {-10, 23}, {4, 19}, {7, 32}, {-11, 36}, {-13, 50}},
    {4, 400, 400, CurveSinEaseInOut, {7, -37}, {7, -18}, {8, -6}, {13, 3}, {8, -6}, {13, 3}, {7, 5}, {8, 18}, {7, 32}, {8, 18}, {7, 32}},
    {4, 400, 0, CurveSinEaseInOut, {5, -16}, {-2, 3}, {6, 9}, {16, 6}, {6, 9}, {16, 6}, {-10, 23}, {4, 19}, {7, 32}, {-11, 36}, {-13, 50}}};
// Squats
static const SkeletonFrame skeleton_data_squats[] = {
    {2, 600, 800, CurveSinEaseInOut, {-4, -30}, {-4, -10}, {7, -6}, {19, -7}, {7, -6}, {19, -7}, {-4, 18}, {-2, 31}, {-4, 46}, {-2, 31}, {-4, 46}},
    {2, 600, 1200, CurveSinEaseInOut, {3, -9}, {-5, 12}, {6, 13}, {17, 12}, {6, 13}, {17, 12}, {-12, 30}, {7, 31}, {-4, 46}, {7, 31}, {-4, 46}}};
// Chair dips
static const SkeletonFrame skeleton_data_chair_dips[] = {
    {2, 600, 400, CurveSinEaseInOut, {2, -25}, {0, -4}, {4, 6}, {6, 18}, {4, 6}, {6, 18}, {-4, 19}, {-20, 20}, {-20, 37}, {-20, 20}, {-20, 37}},
    {2, 600, 400, CurveSinEaseInOut, {0, -13}, {-2, 6}, {9, 8}, {6, 18}, {9, 8}, {6, 18}, {-6, 31}, {-20, 19}, {-20, 37}, {-20, 19}, {-20, 37}}};
// Planks
static const SkeletonFrame skeleton_data_planks[] = {
    {2, 400, 1600, CurveSinEaseInOut, {29, 1}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}},
    {2, 400, 1200, CurveSinEaseInOut, {30, 6}, {8, 3}, {8, 18}, {20, 19}, {8, 18}, {20, 19}, {-16, 8}, {-30, 14}, {-44, 19}, {-30, 14}, {-44, 19}}};
// High knees
static const SkeletonFrame skeleton_data_high_knees[] = {
    {2, 330, 0, CurveLinear, {-3, -30}, {-3, -7}, {-10, 2}, {1, 7}, {0, 4}, {11, 0}, {-3, 17}, {12, 14}, {4, 24}, {-4, 32}, {-6, 46}},
    {2, 330, 0, CurveLinear, {-3, -29}, {-3, -7}, {0, 4}, {11, 0}, {-10, 2}, {1, 7}, {-3, 17}, {-4, 32}, {-6, 46}, {12, 14}, {4, 24}}};
// Lunges
static const SkeletonFrame skeleton_data_lunges[] = {
    {3, 400, 400, CurveSinEaseInOut, {0, -32}, {0, -10}, {-9, -2}, {0, 6}, {-9, -2}, {0, 6}, {0, 16}, {2, 30}, {0, 45}, {2, 30}, {0, 45}},
    {3, 80, 0, CurveSinEaseIn, {0, -29}, {0, -7}, {-9, 1}, {0, 9}, {-9, 1}, {0, 9}, {0, 19}, {7, 25}, {9, 40}, {2, 30}, {0, 45}},
    {3, 400, 400, CurveSinEaseOut, {0, -21}, {0, 0}, {-9, 7}, {0, 15}, {-9, 7}, {0, 15}, {0, 25}, {15, 28}, {15, 45}, {-3, 39}, {-17, 45}}};
// Push-ups and rotations
static const SkeletonFrame skeleton_data_pushups_rotations[] = {
    {2, 400, 400, CurveSinEaseInOut, {28, 13}, {6, 13}, {-2, 20}, {7, 23}, {-2, 20}, {7, 23}, {-9, 13}, {-32, 17}, {-47, 19}, {-32, 17}, {-47, 19}},
    {2, 400, 400, CurveSinEaseInOut, {25, -3}, {4, 1}, {3, 4}, {4, -21}, {5, 12}, {7, 23}, {-21, 6}, {-33, 13}, {-47, 19}, {-33, 13}, {-47, 19}}};
// Side plank
static const SkeletonFrame skeleton_data_side_plank[] = {
    {2, 400, 400, CurveSinEaseInOut, {29, 2}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}},
    {2, 400, 1200, CurveSinEaseInOut, {29, 4}, {7, 6}, {-4, 1}, {-16, 1}, {6, 18}, {11, 19}, {-16, 9}, {-30, 9}, {-44, 12}, {-28, 18}, {-43, 19}}};
// Done
static const SkeletonFrame skeleton_data_done[] = {
    {8, 400, 200, CurveSinEaseOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {8, 400, 200, CurveSinEaseOut, {0, -30}, {0, -10}, {-11, -12}, {-20, -22}, {11, -12}, {20, -22}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {8, 400, 200, CurveSinEaseOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {8, 400, 200, CurveSinEaseOut, {5, -29}, {3, -8}, {-8, -12}, {-18, -21}, {15, -4}, {6, -13}, {2, 17}, {-6, 29}, {-10, 44}, {8, 31}, {10, 44}},
    {8, 400, 200, CurveSinEaseOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {8, 400, 200, CurveSinEaseOut, {0, -30}, {0, -10}, {-11, -12}, {-20, -22}, {11, -12}, {20, -22}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {8, 400, 200, CurveSinEaseOut, {0, -30}, {0, -10}, {-9, -3}, {-13, 9}, {9, -3}, {13, 9}, {0, 15}, {-8, 29}, {-10, 44}, {8, 29}, {10, 44}},
    {8, 400, 200, CurveSinEaseOut, {-5, -29}, {-3, -8}, {-15, -4}, {-6, -13}, {8, -12}, {18, -21}, {-2, 17}, {-8, 31}, {-10, 44}, {6, 29}, {10, 44}}};
// Side plank swapped (nasty little bit of inconsistency in the exercise)
static const SkeletonFrame skeleton_data_side_plank_swapped[] = {
    {2, 400, 400, CurveSinEaseInOut, {-29, 2}, {-7, 6}, {4, 1}, {16, 1}, {-6, 18}, {-11, 19}, {16, 9}, {30, 9}, {44, 12}, {28, 18}, {43, 19}},
    {2, 400, 1200, CurveSinEaseInOut, {-29, 4}, {-7, 6}, {4, 1}, {16, 1}, {-6, 18}, {-11, 19}, {16, 9}, {30, 9}, {44, 12}, {28, 18}, {43, 19}}};
// Array of data animations
static const SkeletonFrame *skeleton_frame_data[] = {
  skeleton_data_rest,
  skeleton_data_waiting,
  skeleton_data_jumping_jacks,
  skeleton_data_wall_sit,
  skeleton_data_pushups,
  skeleton_data_crunches,
  skeleton_data_step_ups,
  skeleton_data_squats,
  skeleton_data_chair_dips,
  skeleton_data_planks,
  skeleton_data_high_knees,
  skeleton_data_lunges,
  skeleton_data_pushups_rotations,
  skeleton_data_side_plank,
  skeleton_data_done,
  skeleton_data_side_plank_swapped
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
//

// Add two points
static GPoint prv_gpoint_add(GPoint pt1, GPoint pt2) {
  return GPoint(pt1.x + pt2.x, pt1.y + pt2.y);
}


// Draw a thick line on the graphics context
static void prv_drawing_thick_line(GContext *ctx, GPoint p0, GPoint p1) {
#ifdef PBL_SDK_3
  graphics_context_set_stroke_width(ctx, 4);
  graphics_draw_line(ctx, p0, p1);
#else
  for (int8_t x = -2; x <= 2; x++) {
    for (int8_t y = -2; y <= 2; y++) {
      if (abs(x) + abs(y) <= 2) {
        graphics_draw_line(ctx, GPoint(p0.x + x, p0.y + y), GPoint(p1.x + x, p1.y + y));
      }
    }
  }
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// API Implementation
//

// Creates a stick figure in memory
StickFigure *stick_figure_create(void) {
  StickFigure *stick_figure = (StickFigure*)malloc(sizeof(StickFigure));
  if (!stick_figure) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create StickFigure");
  }
  return stick_figure;
}


// Destroy a stick figure from memory
void stick_figure_destroy(StickFigure *stick_figure) {
  free(stick_figure);
}


// Get the current pose of a stick figure
StickFigurePose stick_figure_get_pose(StickFigure *stick_figure) {
  return stick_figure->pose;
}


// Set the pose of a stick figure
void stick_figure_set_pose(StickFigure *stick_figure, StickFigurePose pose, int64_t epoch_ms) {
  // set the pose
  stick_figure->pose = pose;
  stick_figure->frame = 0;
  stick_figure->frame_start = epoch_ms;
  // copy current skeleton positions to old skeleton positions
  memcpy(&stick_figure->old_skeleton, &stick_figure->cur_skeleton, sizeof(SkeletonFrame));
}


// Snap the stick figure to a pose
void stick_figure_snap_pose(StickFigure *stick_figure, StickFigurePose pose, int64_t epoch_ms) {
  // set the pose
  stick_figure_set_pose(stick_figure, pose, epoch_ms);
  // copy memory from data into stick figure
  memcpy(&stick_figure->old_skeleton, &skeleton_frame_data[pose][0], sizeof(SkeletonFrame));
  memcpy(&stick_figure->cur_skeleton, &skeleton_frame_data[pose][0], sizeof(SkeletonFrame));
}


// Animates the position of the stick figure skeleton
// between the last position and the current frame
void stick_figure_step_animation(StickFigure *stick_figure, int64_t epoch_ms) {
  // get end skeleton frame
  SkeletonFrame end_skeleton = skeleton_frame_data[stick_figure->pose][stick_figure->frame];
  // step the animation
  if (epoch_ms - stick_figure->frame_start >
      end_skeleton.ani_duration + end_skeleton.pause_duration) {
    // wrap back to first frame
    if (++stick_figure->frame >= end_skeleton.frame_count) {
      stick_figure->frame = 0;
    }
    // prep next for next frame
    stick_figure->frame_start += end_skeleton.ani_duration + end_skeleton.pause_duration;
    memcpy(&stick_figure->old_skeleton, &stick_figure->cur_skeleton, sizeof(SkeletonFrame));
    end_skeleton = skeleton_frame_data[stick_figure->pose][stick_figure->frame];
  }

  // step the skeleton points if in animation stage, else set them to the end point
  if (epoch_ms - stick_figure->frame_start <= end_skeleton.ani_duration) {
    // must be a better way to do this without using an array for the figure's points
    stick_figure->cur_skeleton.head = interpolation_gpoint(stick_figure->old_skeleton.head,
      end_skeleton.head, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.chest = interpolation_gpoint(stick_figure->old_skeleton.chest,
      end_skeleton.chest, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.elbow_r = interpolation_gpoint(stick_figure->old_skeleton.elbow_r,
      end_skeleton.elbow_r, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.hand_r = interpolation_gpoint(stick_figure->old_skeleton.hand_r,
      end_skeleton.hand_r, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.elbow_l = interpolation_gpoint(stick_figure->old_skeleton.elbow_l,
      end_skeleton.elbow_l, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.hand_l = interpolation_gpoint(stick_figure->old_skeleton.hand_l,
      end_skeleton.hand_l, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.waist = interpolation_gpoint(stick_figure->old_skeleton.waist,
      end_skeleton.waist, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.knee_r = interpolation_gpoint(stick_figure->old_skeleton.knee_r,
      end_skeleton.knee_r, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.foot_r = interpolation_gpoint(stick_figure->old_skeleton.foot_r,
      end_skeleton.foot_r, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.knee_l = interpolation_gpoint(stick_figure->old_skeleton.knee_l,
      end_skeleton.knee_l, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
    stick_figure->cur_skeleton.foot_l = interpolation_gpoint(stick_figure->old_skeleton.foot_l,
      end_skeleton.foot_l, (int32_t)(epoch_ms - stick_figure->frame_start),
      end_skeleton.ani_duration, end_skeleton.interp_curve);
  }
  else {
    memcpy(&stick_figure->cur_skeleton, &end_skeleton, sizeof(SkeletonFrame));
  }
}


// Draw the stick figures current pose with an offset
void stick_figure_draw(StickFigure *stick_figure, GContext *ctx, GPoint offset) {
  SkeletonFrame skeleton = stick_figure->cur_skeleton;
  graphics_context_set_fill_color(ctx, GColorBlack);
  // draw body
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.head, offset),
    prv_gpoint_add(skeleton.chest, offset));
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.chest, offset),
    prv_gpoint_add(skeleton.waist, offset));
  // draw head
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, prv_gpoint_add(skeleton.head, offset), 16);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, prv_gpoint_add(skeleton.head, offset), 12);
  // draw right arm
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.chest, offset),
    prv_gpoint_add(skeleton.elbow_r, offset));
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.elbow_r, offset),
    prv_gpoint_add(skeleton.hand_r, offset));
  // draw left arm
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.chest, offset),
    prv_gpoint_add(skeleton.elbow_l, offset));
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.elbow_l, offset),
    prv_gpoint_add(skeleton.hand_l, offset));
  // draw right leg
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.waist, offset),
    prv_gpoint_add(skeleton.knee_r, offset));
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.knee_r, offset),
    prv_gpoint_add(skeleton.foot_r, offset));
  // draw left leg
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.waist, offset),
    prv_gpoint_add(skeleton.knee_l, offset));
  prv_drawing_thick_line(ctx, prv_gpoint_add(skeleton.knee_l, offset),
    prv_gpoint_add(skeleton.foot_l, offset));
}


// Draw any props that need to go with the stick figure
void stick_figure_draw_props(GContext *ctx, StickFigurePose pose, GPoint offset) {
  // draw props based on current pose with no animation
  switch (pose) {
    case PoseWallSit:
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(-12, -44), offset),
                             prv_gpoint_add(GPoint(-12, 41), offset));
      break;
    case PoseStepUps:
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(3, 51), offset),
                             prv_gpoint_add(GPoint(3, 36), offset));
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(18, 36), offset),
                             prv_gpoint_add(GPoint(3, 36), offset));
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(18, 51), offset),
                             prv_gpoint_add(GPoint(18, 36), offset));
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(22, 17), offset),
                             prv_gpoint_add(GPoint(18, 36), offset));
      break;
    case PoseCharDips:
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(4, 37), offset),
                             prv_gpoint_add(GPoint(4, 22), offset));
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(19, 22), offset),
                             prv_gpoint_add(GPoint(4, 22), offset));
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(19, 37), offset),
                             prv_gpoint_add(GPoint(19, 22), offset));
      prv_drawing_thick_line(ctx, prv_gpoint_add(GPoint(23, 3), offset),
                             prv_gpoint_add(GPoint(19, 22), offset));
      break;
    default:
      break;
  }
}
