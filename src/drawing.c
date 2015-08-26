// @file drawing.h
// @brief General purpose drawing code.
//
// Contains code to draw background, button, and progress bar.
// Also used to draw text on window.
//
// @author Eric D. Phillips
// @bug No know bugs

#include <pebble.h>
#include "drawing.h"
#include "stick_figure.h"
#include "interpolation.h"

#define CIRCLE_RADIUS 60

#define BUTTON_ANIMATION_DURATION 180 // milliseconds

#define EXERCISE_ACTIVITY_PERIOD 30000 // length of an activity is in milliseconds

#ifdef PBL_COLOR
#define COLOR_BACKGROUND GColorDarkGray
#define COLOR_PLAYING_FORE GColorOrange
#define COLOR_PLAYING_BACK GColorPastelYellow
#define COLOR_PAUSED_FORE GColorGreen
#define COLOR_PAUSED_BACK GColorMintGreen
#define COLOR_RESTING_FORE GColorVividCerulean
#define COLOR_RESTING_BACK GColorCeleste
#else
#define COLOR_BACKGROUND GColorBlack
#define COLOR_PLAYING_FORE GColorBlack
#define COLOR_PLAYING_BACK GColorWhite
#define COLOR_PAUSED_FORE GColorBlack
#define COLOR_PAUSED_BACK GColorWhite
#define COLOR_RESTING_FORE GColorBlack
#define COLOR_RESTING_BACK GColorWhite
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Data
//

// Names for stick figure poses
static const char *StickFigureActivityName[] = {"Rest", "First: Jumping Jacks", "Jumping Jacks",
  "Wall Sit", "Pushups", "Crunches", "Step Ups", "Squats", "Chair Dips", "Planks", "High Knees",
  "Lunges", "Pushups & Rotations", "Side Planks", "Done!"};
static const char *StickFigureRestName[] = {"Rest", "First: Jumping Jacks", "Next: Wall Sit",
"Next: Pushups", "Next: Crunches", "Next: Step Ups", "Next: Squats", "Next: Chair Dips",
  "Next: Planks", "Next: High Knees", "Next: Lunges", "Next: Pushups & Rotations",
  "Next: Side Planks", "Done!"};

// Button data
static const GPoint ButtonPlay[] = {{40, 0}, {-20, 34}, {-20, 0}, {-20, -34}};
static const GPoint ButtonPauseL[] = {{-30, -30}, {-30, 30}, {-10, 30}, {-10, -30}};
static const GPoint ButtonPauseR[] = {{10, -30}, {30, -30}, {30, 30}, {10, 30}};

// Structure for button
typedef struct Button {
  GPoint    left[4];      //!< Point data for left bar of pause button
  GPoint    right[4];     //!< Point data for right bar of pause button
  int64_t   click_epoch;  //!< Epoch in milliseconds of the last button press
  bool      playing;      //!< The state of the button, with true being playing
  bool      ani_done;     //!< If the animation is done and the exact points are set
} Button;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
//

// Get a point from a center point, angle, and radius
static GPoint prv_polar_to_rectangular(GPoint center, uint32_t angle, uint16_t radius) {
  // get the point locations
  int32_t X = (-sin_lookup(angle) * radius / TRIG_MAX_RATIO) + center.x;
  int32_t Y = (-cos_lookup(angle) * radius / TRIG_MAX_RATIO) + center.y;
  // add them to the path
  return GPoint(X, Y);
}


// Draw the gray cover over part of the progress ring
static void prv_draw_progress_ring(GContext *ctx, GSize window_size, uint32_t angle) {
  graphics_context_set_fill_color(ctx, COLOR_BACKGROUND);
  // get step angle and exit if too small
  int32_t step = angle / 4;
  if (step < 1) {
    return;
  }
  // get properties
  GPoint center = GPoint(window_size.w / 2, window_size.h / 2);
  uint16_t radius = window_size.h + window_size.w / 2;
  // calculate points around outside of window to draw cover
  GPoint points[8];
  int8_t idx = 0;
  for (uint32_t t_angle = 0; t_angle < angle; t_angle += step){
    points[idx++] = prv_polar_to_rectangular(center, t_angle, radius);
  }
  // add point at hand position, and in center (to form pie wedge)
  points[idx++] = prv_polar_to_rectangular(center, angle, radius);
  points[idx++] = center;

  // fill the covering
  GPathInfo info = (GPathInfo) {
    .num_points = idx,
    .points = points
  };
  GPath *path = gpath_create(&info);
  gpath_draw_filled(ctx, path);
  gpath_destroy(path);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// API Implementation
//

// Create a button
Button *drawing_button_create(void) {
  Button *button = (Button*)malloc(sizeof(Button));
  if (button) {
    memcpy(button->left, ButtonPlay, sizeof(ButtonPlay));
    memcpy(button->right, ButtonPlay, sizeof(ButtonPlay));
    button->click_epoch = 0;
    button->playing = false;
    button->ani_done = true;
    return button;
  }

  // error handling
  APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create button");
  window_stack_pop_all(false);
  return NULL;
}


// Destroy a button
void drawing_button_destroy(Button *button) {
  free(button);
}


// Set button state
void drawing_button_set_state(Button *button, int64_t epoch_ms, bool playing) {
  button->click_epoch = epoch_ms;
  button->playing = playing;
  button->ani_done = false;
}


// Step button animation
void drawing_button_step_animation(Button *button, int64_t epoch_ms) {
  if (!button->ani_done) {
    // check timing of animation
    if (epoch_ms - button->click_epoch > BUTTON_ANIMATION_DURATION) {
      // pretend the last click was exactly the animation duration ago
      // this sets the points to exactly the correct places for the new button state
      button->click_epoch = epoch_ms - BUTTON_ANIMATION_DURATION;
      button->ani_done = true;
    }
    // loop through the points in the button
    for (uint8_t idx = 0; idx < ARRAY_LENGTH(button->right); idx++) {
      if (button->playing) {
        button->right[idx] = interpolation_gpoint(ButtonPauseR[idx], ButtonPlay[idx],
            epoch_ms - button->click_epoch, BUTTON_ANIMATION_DURATION, CurveSinEaseInOut);
        button->left[idx] = interpolation_gpoint(ButtonPauseL[idx], ButtonPlay[idx],
            epoch_ms - button->click_epoch, BUTTON_ANIMATION_DURATION, CurveSinEaseInOut);
      } else {
        button->right[idx] = interpolation_gpoint(ButtonPlay[idx], ButtonPauseR[idx],
            epoch_ms - button->click_epoch, BUTTON_ANIMATION_DURATION, CurveSinEaseInOut);
        button->left[idx] = interpolation_gpoint(ButtonPlay[idx], ButtonPauseL[idx],
            epoch_ms - button->click_epoch, BUTTON_ANIMATION_DURATION, CurveSinEaseInOut);
      }
    }
  }
}


// Draw the button onto the drawing context
void drawing_button_draw(Button *button, GContext *ctx, GSize window_size, uint8_t pose,
                         GBitmap *bmp) {
  // set fill color
  GColor fore_color = COLOR_PLAYING_FORE;
  if (pose == PoseWaitingForStart || pose == PoseDone) {
    fore_color = COLOR_PAUSED_FORE;
  } else if (pose == PoseResting) {
    fore_color = COLOR_RESTING_FORE;
  }
  graphics_context_set_fill_color(ctx, fore_color);
  // draw as path
  GPathInfo button_path = {
    .num_points = 4,
    .points = button->right,
  };
  GPath *path = gpath_create(&button_path);
  gpath_move_to(path, GPoint(window_size.w / 2, window_size.h / 2));
  gpath_draw_filled(ctx, path);
  gpath_destroy(path);
  button_path.points = button->left;
  path = gpath_create(&button_path);
  gpath_move_to(path, GPoint(window_size.w / 2, window_size.h / 2));
  gpath_draw_filled(ctx, path);
  gpath_destroy(path);
  // if aplite, cover with gray
#ifndef PBL_COLOR
  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  graphics_draw_bitmap_in_rect(ctx, bmp, GRect(32, 42, 80, 80));
#endif
}


// Draw background in center of window
void drawing_background(GContext *ctx, GSize window_size, uint32_t angle, uint8_t pose,
                        GBitmap *bmp) {
#ifdef PBL_COLOR
  // draw background color of window
  // this is actually the color of the progress ring, which is then covered up to give
  // the appearance of progress
  GColor fore_color = COLOR_PLAYING_FORE;
  GColor back_color = COLOR_PLAYING_BACK;
  if (pose == PoseWaitingForStart || pose == PoseDone) {
    fore_color = COLOR_PAUSED_FORE;
    back_color = COLOR_PAUSED_BACK;
  }
  else if (pose == PoseResting) {
    fore_color = COLOR_RESTING_FORE;
    back_color = COLOR_RESTING_BACK;
  }
  graphics_context_set_fill_color(ctx, fore_color);
  graphics_fill_rect(ctx, GRect(0, 0, window_size.w, window_size.h), 1, GCornerNone);
#else
  GColor back_color = COLOR_PLAYING_BACK;
  graphics_draw_bitmap_in_rect(ctx, bmp, GRect(0, 0, window_size.w, window_size.h));
#endif
  // draw the cover for the progress ring
  prv_draw_progress_ring(ctx, window_size, angle);
  // draw the center circle
  graphics_context_set_fill_color(ctx, back_color);
  graphics_fill_circle(ctx, GPoint(window_size.w / 2, window_size.h / 2), CIRCLE_RADIUS);
}


// Draw text onto the image
void drawing_text(GContext *ctx, GSize window_size, uint8_t activity, int32_t period_time,
                  bool in_activity, bool unstarted) {
#ifdef PBL_COLOR
  graphics_context_set_text_color(ctx, GColorBlack);
#else
  graphics_context_set_text_color(ctx, GColorWhite);
#endif
  const char *buff;
  if (unstarted) {
    buff = StickFigureRestName[PoseWaitingForStart];
  }
  else if (in_activity) {
    buff = StickFigureActivityName[activity];
  }
  else {
    buff = StickFigureRestName[activity];
  }
  graphics_draw_text(ctx, buff, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
    GRect(0, window_size.h - 20, window_size.w, 20), GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter, NULL);

  // draw "Switch" half way through the side planks
  if (activity == 13 && period_time >= EXERCISE_ACTIVITY_PERIOD / 2 &&
      period_time < EXERCISE_ACTIVITY_PERIOD / 2 + 5000) {
    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(ctx, "Switch", fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
                       GRect(0, window_size.h / 2 + 35, window_size.w, 20),
                       GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}


// Draw configure on phone dialog
void drawing_config(GContext *ctx, GSize window_size, GBitmap *bmp) {
  // set drawing colors
  graphics_context_set_fill_color(ctx, COLOR_PAUSED_FORE);
#ifdef PBL_COLOR
  graphics_context_set_text_color(ctx, GColorBlack);
#else
  graphics_context_set_text_color(ctx, GColorWhite);
#endif
  // draw background
  graphics_fill_rect(ctx, GRect(0, 0, window_size.w, window_size.h), 1, GCornerNone);
  // draw image
  GRect bmp_size = gbitmap_get_bounds(bmp);
#ifdef PBL_COLOR
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
#endif
  graphics_draw_bitmap_in_rect(ctx, bmp, GRect((window_size.w - bmp_size.size.w) / 2,
                                               (window_size.h - bmp_size.size.h) / 2 - 5,
                                               bmp_size.size.w, bmp_size.size.h));
  // draw title
  graphics_draw_text(ctx, "7 Minute+", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
                     GRect(0, 7, window_size.w, 28), GTextOverflowModeFill,
                     GTextAlignmentCenter, NULL);
  // draw footer
  graphics_draw_text(ctx, "Set up in the Pebble app",
                     fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                     GRect(10, 120, window_size.w - 20, 48), GTextOverflowModeWordWrap,
                     GTextAlignmentCenter, NULL);
}
