//! @file main.c
//! @brief Main logic for 7-Min
//!
//! File contains all logic for 7-Min Workout Pebble application.
//! The program runs much like a timer, simply logging the start time, offsetting it each time
//! it is paused or resumed, and then calculating all figure states based off the difference
//! between the current time and the start time.
//!
//! @author Eric D. Phillips
//! @bug No known bugs

#include <pebble.h>
#include "stick_figure.h"
#include "drawing.h"

#define KEY_EXERCISED 5 // key used when sending app message to phone

#define REFRESH_RATE_FPS 30
#define REFRESH_RATE_MS_PER_FRAME 1000 / REFRESH_RATE_FPS
#define EXERCISE_ACTIVITY_PERIOD 30000 // length of an activity is in milliseconds
#define EXERCISE_REST_PERIOD 10000 // length of a rest is in milliseconds
#define EXERCISE_TOTAL_PERIOD (EXERCISE_ACTIVITY_PERIOD + EXERCISE_REST_PERIOD) // milliseconds
#define EXERCISE_ACTIVITY_COUNT 12
#define EXERCISE_OVERLAP_TIME 1000 // the max length of time into an exercise to step back to the
                                   // previous exercise rather than restarting that exercise

//! WindowData structure containing all data and stored in the window's user data
typedef struct WindowData {
  Window        *window;        //!< Pointer to the window the data is attached to
  Layer         *draw_layer;    //!< Pointer to layer on which most things are drawn

  StickFigure   *stick_figure;  //!< Pointer to the stick figure which will be used
  Button        *button;        //!< Pointer to the button in the center of the screen
  bool          manual_change;  //!< Set to true when changing the pose to avoid vibrating

  int64_t       start_epoch;    //!< The epoch in milliseconds when the workout was started
  AppTimer      *app_timer;     //!< Main timer for refreshing
} WindowData;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
//

//! Get the current epoch time in milliseconds
//! @return The current epoch in milliseconds
static int64_t prv_get_epoch_ms(void) {
  time_t sec;
  uint16_t msec;
  time_ms(&sec, &msec);
  return (int64_t)sec * 1000 + (int64_t)msec;
}


//! Gets the timing of the current exercise
//! @param data The window user data
//! @param epoch_ms The current epoch in milliseconds
//! @param period_time A pointer to an int, an output which is the time into the current exercise
//! @param period_time_total A pointer to an int, an output which is the current exercise duration
static void prv_get_current_timing(WindowData *data, int64_t epoch_ms, int32_t *run_time,
                                   int32_t *period_time, int32_t *period_time_total) {
  // get current time into the workout in milliseconds
  (*run_time) = (int32_t)(epoch_ms - (((data->start_epoch + epoch_ms - 1) % epoch_ms) + 1));
  // get amount of time into current period, a period is either an activity or resting
  (*period_time) = ((*run_time) % EXERCISE_TOTAL_PERIOD) % EXERCISE_ACTIVITY_PERIOD;
  // get total duration of current period
  (*period_time_total) = ((*run_time) % EXERCISE_TOTAL_PERIOD < EXERCISE_ACTIVITY_PERIOD) ?
                         EXERCISE_ACTIVITY_PERIOD : EXERCISE_REST_PERIOD;
}


//! Sends a message to the phone to add a timeline pin
static void prv_phone_send_pin(void) {
  // begin iterator
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  // write data
  dict_write_uint8(iter, KEY_EXERCISED, 0);
  dict_write_end(iter);
  // send
  int res = app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Pin Sent: %d", res);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks
//

//! Callback for AppTimer
//! @param callback_data The data associated with the timer
static void prv_app_timer_callback(void *callback_data) {
  WindowData *data = (WindowData*)callback_data;
  // refresh layer
  layer_mark_dirty(data->draw_layer);
  // schedule next call
  data->app_timer = app_timer_register(REFRESH_RATE_MS_PER_FRAME, prv_app_timer_callback, data);
}


//! Callback for drawing layer to render visuals in
//! @param layer The layer being updated.
//! @param ctx The GContext to draw on
static void prv_layer_update_proc_handler(Layer *layer, GContext *ctx) {
  // get data
  WindowData *data = (*(WindowData**)layer_get_data(layer));
  GSize window_size = layer_get_bounds(layer).size;
  GPoint offset = GPoint(window_size.w / 2, window_size.h / 2);
  int64_t epoch_ms = prv_get_epoch_ms();

  // step animations
  stick_figure_step_animation(data->stick_figure, epoch_ms);
  drawing_button_step_animation(data->button, epoch_ms);

  // get timing values
  int32_t run_time, period_time, period_time_total;
  prv_get_current_timing(data, epoch_ms, &run_time, &period_time, &period_time_total);
  uint32_t angle = TRIG_MAX_ANGLE * period_time / period_time_total;

  // get current pose
  StickFigurePose cur_pose = (StickFigurePose)(run_time / EXERCISE_TOTAL_PERIOD +
                                               PoseJumpingJacks);
  if (data->start_epoch <= 0) {
    cur_pose = PoseWaitingForStart;
  } else if (run_time >= EXERCISE_TOTAL_PERIOD * EXERCISE_ACTIVITY_COUNT - EXERCISE_REST_PERIOD) {
    data->start_epoch = epoch_ms - EXERCISE_TOTAL_PERIOD * EXERCISE_ACTIVITY_COUNT;
    cur_pose = PoseDone;
    angle = 0;
  } else if (period_time_total == EXERCISE_REST_PERIOD) {
    cur_pose = PoseResting;
  }
  StickFigurePose old_pose = stick_figure_get_pose(data->stick_figure);
  if (cur_pose != old_pose) {
    // vibrate if not at start or end
    if (!data->manual_change){
      vibes_short_pulse();
      // send a pin if last exercise
      if (cur_pose == PoseDone) {
        prv_phone_send_pin();
      }
    }
    // set the new pose
    stick_figure_set_pose(data->stick_figure, cur_pose, epoch_ms);
    printf("Value: %d", (int)cur_pose);
  }
  // set manual change to false to enable vibrations again
  data->manual_change = false;

  // draw the visuals on the screen
  drawing_background(ctx, window_size, angle, cur_pose, NULL);
  drawing_button_draw(data->button, ctx, window_size, cur_pose);
  stick_figure_draw(data->stick_figure, ctx, offset);
  stick_figure_draw_props(ctx, cur_pose, offset);
  // draw text
  uint8_t cur_activity = run_time / EXERCISE_TOTAL_PERIOD + PoseJumpingJacks;
  if (cur_pose == PoseDone) {
    cur_activity = PoseDone;
  }
  drawing_text(ctx, window_size, cur_activity, period_time_total == EXERCISE_ACTIVITY_PERIOD,
               run_time == 1);
}


//! "Up" click handler
//! @param recognizer Click recognizer
//! @param context User specified data passed into each click callback
static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  WindowData *data = (WindowData*)context;
  // get current timings
  int64_t epoch_ms = prv_get_epoch_ms();
  int32_t run_time, period_time, period_time_total;
  prv_get_current_timing(data, epoch_ms, &run_time, &period_time, &period_time_total);
  // move to start of current activity or previous activity if within threshold
  data->start_epoch += period_time;
  if (period_time < EXERCISE_OVERLAP_TIME &&
      run_time >= EXERCISE_TOTAL_PERIOD - period_time_total &&
      run_time < EXERCISE_ACTIVITY_COUNT * EXERCISE_TOTAL_PERIOD) {
    data->start_epoch += EXERCISE_TOTAL_PERIOD - period_time_total;
    // set manual change to prevent vibration
    data->manual_change = true;
  }
}


//! "Select" click handler
//! @param recognizer Click recognizer
//! @param context User specified data passed into each click callback
static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  WindowData *data = (WindowData*)context;
  // get current epoch in milliseconds
  int64_t epoch_ms = prv_get_epoch_ms();
  bool paused = data->start_epoch <= 0;
  // play and pause the workout
  if (paused) {
    data->start_epoch += epoch_ms;
  } else {
    data->start_epoch -= epoch_ms;
    // reset if at end
    if (stick_figure_get_pose(data->stick_figure) == PoseDone) {
      data->start_epoch = 0;
    }
  }
  // change button state
  drawing_button_set_state(data->button, epoch_ms, !paused);
  // set manual change to prevent vibration
  data->manual_change = true;
}


//! "Down" click handler
//! @param recognizer Click recognizer
//! @param context User specified data passed into each click callback
static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  WindowData *data = (WindowData*)context;
  // get current timings
  int64_t epoch_ms = prv_get_epoch_ms();
  int32_t run_time, period_time, period_time_total;
  prv_get_current_timing(data, epoch_ms, &run_time, &period_time, &period_time_total);
  // move to start of current activity or previous activity if within threshold
  if (run_time < EXERCISE_ACTIVITY_COUNT * EXERCISE_TOTAL_PERIOD) {
    data->start_epoch -= period_time_total - period_time;
    // set manual change to prevent vibration
    data->manual_change = true;
  }
}


//! Click configuration provider
//! @param context User specified data to associate with click configuration
static void prv_click_config_provider(void *context) {
  // set click context for each button
  window_set_click_context(BUTTON_ID_UP, context);
  window_set_click_context(BUTTON_ID_SELECT, context);
  window_set_click_context(BUTTON_ID_DOWN, context);
  // set button click callbacks
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 50, prv_down_click_handler);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Loading and Unloading
//

//! Create window data and layers in here, as well as any variable prepping
//! @param window A pointer to the window the load was called by
static void prv_window_load_handler(Window *window) {
  // window properties
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  WindowData *data = (WindowData*)malloc(sizeof(WindowData));
  // check return
  if (data) {
    // set window properties
    window_set_user_data(window, data);
    window_set_click_config_provider_with_context(window, prv_click_config_provider, data);
    // create drawing layer and store pointer to window data in it
    data->draw_layer = layer_create_with_data(bounds, sizeof(StickFigure*));
    WindowData **layer_data = (WindowData**)layer_get_data(data->draw_layer);
    (*layer_data) = data;
    layer_set_update_proc(data->draw_layer, prv_layer_update_proc_handler);
    layer_add_child(root, data->draw_layer);

    // create the stick figure
    data->stick_figure = stick_figure_create();
    stick_figure_snap_pose(data->stick_figure, PoseWaitingForStart, prv_get_epoch_ms());

    // create the button
    data->button = drawing_button_create();

    // set some window data properties
    data->start_epoch = 0;

    // open app message communication with the phone
    app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);

    // start first refresh
    data->app_timer = app_timer_register(REFRESH_RATE_MS_PER_FRAME, prv_app_timer_callback, data);
    return;
  }

  // error handling
  window_stack_remove(window, false);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error: Failed to allocate memory for WindowData.");
}


//! Destroy layers, window data, and window.
//! @param window A pointer to the window the unload was called from
static void prv_window_unload_handler(Window *window) {
  // window properties
  WindowData *data = (WindowData*)window_get_user_data(window);
  // check user data
  if (data) {
    // free data
    stick_figure_destroy(data->stick_figure);
    drawing_button_destroy(data->button);
    layer_destroy(data->draw_layer);
    window_destroy(window);
    free(data);
    return;
  }

  // error handling
  window_destroy(window);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error: Tried to free NULL WindowData pointer.");
}


//! Initialize Window object
static void initialize(void) {
  Window *window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load_handler,
    .unload = prv_window_unload_handler,
  });
  window_stack_push(window, true);
}


//! Entry point
//! @return Should not return
int main(void) {
  initialize();
  app_event_loop();
}
