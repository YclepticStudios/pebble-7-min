//! @file drawing.h
//! @brief General purpose drawing code.
//!
//! Contains code to draw background, button, and progress bar.
//! Also used to draw text on window.
//!
//! @author Eric D. Phillips
//! @bug No know bugs

#pragma once
#include <pebble.h>

//! Structure for button data
typedef struct Button Button;

//! Create a Button
//! @return A pointer to the newly allocated button
Button *drawing_button_create(void);

//! Destroy a previously created button
//! @param button A pointer to the button to destroy
void drawing_button_destroy(Button *button);

//! Set the state of the button
//! @param button A pointer to the button for which to set the state
//! @param epoch_ms The current epoch in milliseconds
//! @param playing The state of the button, true is playing, false is paused
void drawing_button_set_state(Button *button, int64_t epoch_ms, bool playing);

//! Step the animation of the button
//! @param button A pointer to the button for which to step the animation
//! @param epoch_ms The current epoch in milliseconds
void drawing_button_step_animation(Button *button, int64_t epoch_ms);

//! Draw the button onto the drawing context
//! @param button A pointer to the button to draw
//! @param ctx A pointer to the graphics context to draw the button on
//! @param window_size The size of the window being drawn on
//! @param pose The current pose of the stick figure
void drawing_button_draw(Button *button, GContext *ctx, GSize window_size, uint8_t pose);

//! Draw background
//! @param ctx Graphics context to draw on
//! @param bmp Bitmap to draw as background grid for Aplite
//! @param window_size Size of the window on which to draw
//! @param angle Angle that that the gray cover should be drawn at
void drawing_background(GContext *ctx, GSize window_size, uint32_t angle, uint8_t pose,
                        GBitmap *bmp);

//! Draw text on the image
//! @param ctx Graphics context to draw on
//! @param window_size The size of the window being drawn on
//! @param activity The number of the activity currently in
//! @param in_activity If it is currently in one of the activities, not resting
//! @param unstarted If it is still at the first screen and unstarted
void drawing_text(GContext *ctx, GSize window_size, uint8_t activity, bool in_activity,
                  bool unstarted);

//! Draw configure on phone dialog
//! @param ctx Graphics context to draw on
//! @param window_size The size of the window being drawn on
//! @param bmp The image to draw in the center of this display
void drawing_config(GContext *ctx, GSize window_size, GBitmap *bmp);
