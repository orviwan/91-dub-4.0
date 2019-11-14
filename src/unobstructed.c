#include <pebble.h>
#include "_globals.h"
#include "unobstructed.h"
#include "helpers.h"
#include "settings.h"
#include "window.h"
#include "decorations.h"

static bool s_screen_is_obstructed;

// Event fires once, before the obstruction appears or disappears
void prv_unobstructed_will_change(GRect final_unobstructed_screen_area, void *context) {
  if (s_screen_is_obstructed) {
    // Obstruction is about disappear
  } else {
    // Obstruction is about to appear
    // Hide the things
    decorations_toggle(true);
  }
}

// Event fires once, after obstruction appears or disappears
void prv_unobstructed_did_change(void *context) {
  // Keep track if the screen is obstructed or not
  s_screen_is_obstructed = !s_screen_is_obstructed;

  if(!s_screen_is_obstructed) {
    // Show the things
    decorations_toggle(false);
  }

}

// Event fires frequently, while obstruction is appearing or disappearing
void prv_unobstructed_change(AnimationProgress progress, void *context) {
  // Current unobstructed window size
  GRect full_bounds = layer_get_bounds(my_window_layer);
  GRect bounds = layer_get_unobstructed_bounds(my_window_layer);
  int diff = (full_bounds.size.h - bounds.size.h) / 2;

  GRect frame = layer_get_frame(my_window_layer);
  frame.origin.y = diff * -1;
  layer_set_frame(my_window_layer, frame);
}

void unobstructed_init() {
  GRect fullscreen = layer_get_bounds(my_window_layer);
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(my_window_layer);

  // Determine if the screen is obstructed when the app starts
  s_screen_is_obstructed = !grect_equal(&fullscreen, &unobstructed_bounds);

  decorations_toggle(s_screen_is_obstructed);
  prv_unobstructed_change(100, NULL);

  // Subscribe to the unobstructed area events
  UnobstructedAreaHandlers handlers = {
    .will_change = prv_unobstructed_will_change,
    .change = prv_unobstructed_change,
    .did_change = prv_unobstructed_did_change
  };
  unobstructed_area_service_subscribe(handlers, NULL);
}

void unobstructed_deinit() {
  unobstructed_area_service_unsubscribe();
}
