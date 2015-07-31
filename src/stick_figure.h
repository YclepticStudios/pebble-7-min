//! @file stick_figure.h
//! @brief Code to draw and animate a stick figure
//!
//! File contains code necessary to draw a stick figure onto the graphics context
//! as well as code necessary to animate the model between different poses.
//! This is done by having end positions for each movement and interpolating between them
//! using different easing functions. Each intermediate step is then drawn to give a much
//! smoother animation using far fewer actual data frames.
//! All data is stored in the code.
//!
//! @author Eric D. Phillips
//! @bug No know bugs

#pragma once
#include <pebble.h>

//! exported symbol for stick figure structure
typedef struct StickFigure StickFigure;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Exposed Stances
//

//! An enumerated list of poses that the stick figure can transform into
typedef enum StickFigurePose {
  PoseResting,
  PoseWaitingForStart,
  PoseJumpingJacks,
  PoseWallSit,
  PosePushups,
  PoseCrunches,
  PoseStepUps,
  PoseSquats,
  PoseCharDips,
  PosePlanks,
  PoseHighKnees,
  PoseLunges,
  PosePushupsAndRotations,
  PoseSidePlanks,
  PoseDone
} StickFigurePose;


////////////////////////////////////////////////////////////////////////////////////////////////////
// API Implementation
//

//! Create a stick figure in memory
//! By default this is initialized to the standing pose
//! @return A pointer to the newly allocated StickFigure
StickFigure *stick_figure_create(void);

//! Destroy a stick figure from memory
//! @param stick_figure A pointer to a previously allocated StickFigure
void stick_figure_destroy(StickFigure *stick_figure);

//! Get the current pose of a stick figure
//! @param stick_figure A pointer to the stick figure from which to get the pose
//! @return the current stick figure pose
StickFigurePose stick_figure_get_pose(StickFigure *stick_figure);

//! Set the pose of a stick figure
//! @param stick_figure A pointer to the stick figure for which to set the pose
//! @param pose The pose to which the stick figure should be set
//! @param epoch_ms The current epoch time in milliseconds
void stick_figure_set_pose(StickFigure *stick_figure, StickFigurePose pose, int64_t epoch_ms);

//! Snap to a particular pose
//! @param stick_figure A pointer to the stick figure for which to set the pose
//! @param pose The pose to which the stick figure should be set
//! @param epoch_ms The current epoch time in milliseconds
void stick_figure_snap_pose(StickFigure *stick_figure, StickFigurePose pose, int64_t epoch_ms);

//! Step animation between posses
//! @param stick_figure A pointer to the stick figure for which to set the pose
//! @param epoch_ms The current epoch time in milliseconds
void stick_figure_step_animation(StickFigure *stick_figure, int64_t epoch_ms);

//! Draw a stick figure onto a graphics context
//! @param ctx The graphics context to draw the figure onto
//! @param stick_figure The stick figure to draw onto the graphics context
//! @param offset The offset with which to draw the stick figure
void stick_figure_draw(StickFigure *stick_figure, GContext *ctx, GPoint offset);

//! Draw any props needed by the stick figure
//! @param ctx The graphics context that will be drawn onto
//! @param pose The current stick figure pose that the props will be drawn for
//! @param offset The offset with which to draw the stick figure
void stick_figure_draw_props(GContext *ctx, StickFigurePose pose, GPoint offset);
