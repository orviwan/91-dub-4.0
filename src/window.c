#include <pebble.h>
#include "_globals.h"
#include "window.h"

Window *my_window;
Layer *my_window_layer, *my_shifting_layer;

void window_init() {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "launch_reason() %d", (int)launch_reason());

  my_window = window_create();
  my_window_layer = window_get_root_layer(my_window);

  window_set_background_color(my_window, GColorBlack);
  window_stack_push(my_window, false);
}

void window_deinit() {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "window_deinit()");

  layer_remove_from_parent(my_window_layer);
  layer_destroy(my_window_layer);
}
