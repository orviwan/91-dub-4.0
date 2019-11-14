#include <pebble.h>
#include "_globals.h"
#include "bluetooth.h"
#include "settings.h"
#include "helpers.h"
#include "window.h"

static Layer *bluetooth_layer, *bluetooth_icon_layer, *bluetooth_circle_layer;

static bool IsBluetoothConnected = true;

static GPath *bt_path_ptr = NULL;
static GPathInfo BT_PATH_INFO = {
  .num_points = 18,
  .points = (GPoint []) {{0, 1}, {2, 3}, {3, 3}, {3, 0}, {4, 0}, {6, 2}, {4, 4}, {6, 6}, {4, 8}, {3, 8}, {3, 5}, {2, 5}, {0, 7}, {2, 5}, {3, 5}, {3, 3}, {2, 3}, {0, 1}}
};

/*
bl1 Bluetooth Circle Connected
bl2 Bluetooth Icon Connected
bl3 Bluetooth Circle Disconnected
bl4 Bluetooth Icon Disconnected
*/
void bluetooth_settings_callback() {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "bluetooth_settings_callback()");
  layer_mark_dirty(bluetooth_layer);
}

void bluetooth_circle_layer_update_callback(Layer *my_layer, GContext* ctx) {
  GColor color = color_helper(colors[c_bl1], global_settings.Invert);
  if(!IsBluetoothConnected) {
    color = color_helper(colors[c_bl3], global_settings.Invert);
  }
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_circle(ctx, GPoint(5,5), 5);
}

void bluetooth_layer_update_callback(Layer *my_layer, GContext* ctx) {
  GColor color = color_helper(colors[c_bl2], global_settings.Invert);
  if(!IsBluetoothConnected) {
    color = color_helper(colors[c_bl4], global_settings.Invert);
  }
  graphics_context_set_stroke_color(ctx, color);
  gpath_draw_outline(ctx, bt_path_ptr);
}

void bluetooth_icon_toggle(uint8_t BluetoothVibe) {
  if(appStarted && !IsBluetoothConnected) {
    if(BluetoothVibe && !powerSaveEngaged) {
      vibes_long_pulse();
    }
    #ifndef PBL_COLOR
      layer_set_hidden(bluetooth_circle_layer, true);
      layer_set_hidden(bluetooth_icon_layer, true);
    #endif
  }
  else if(IsBluetoothConnected) {
    #ifndef PBL_COLOR
      layer_set_hidden(bluetooth_circle_layer, false);
      layer_set_hidden(bluetooth_icon_layer, false);
    #endif
  }
  layer_mark_dirty(bluetooth_circle_layer);
  layer_mark_dirty(bluetooth_icon_layer);
}

void bluetooth_connection_callback(bool connected) {
  IsBluetoothConnected = connected;
  bluetooth_icon_toggle(global_settings.BluetoothVibe);
}

void bluetooth_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "bluetooth_init()");

  bluetooth_layer = layer_create(BLUETOOTH_LAYER);
  //layer_set_hidden(bluetooth_layer, true);
  layer_add_child(my_window_layer, bluetooth_layer);
  
  bluetooth_circle_layer = layer_create(BLUETOOTH_ICON_CIRCLE);
  layer_set_update_proc(bluetooth_circle_layer, bluetooth_circle_layer_update_callback);
  layer_add_child(bluetooth_layer, bluetooth_circle_layer);
  
  bt_path_ptr = gpath_create(&BT_PATH_INFO);
  bluetooth_icon_layer = layer_create(BLUETOOTH_ICON_SYMBOL);
  layer_set_update_proc(bluetooth_icon_layer, bluetooth_layer_update_callback);
  layer_add_child(bluetooth_layer, bluetooth_icon_layer);

  bluetooth_connection_service_subscribe(bluetooth_connection_callback);
  
  bluetooth_connection_callback(bluetooth_connection_service_peek());

  settings_register_callback(bluetooth_settings_callback, SETTINGS_CALLBACK_BLUETOOTH);
  
  //animation_slide_in(bluetooth_layer, 1200, DOWN);
}

void bluetooth_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "bluetooth_deinit()");


  settings_unregister_callback(SETTINGS_CALLBACK_BLUETOOTH);
  bluetooth_connection_service_unsubscribe();
  gpath_destroy(bt_path_ptr);
  bt_path_ptr = NULL;
  
  layer_remove_from_parent(bluetooth_circle_layer);
  layer_remove_from_parent(bluetooth_icon_layer);
  layer_remove_from_parent(bluetooth_layer);
  
  layer_destroy(bluetooth_circle_layer);
  layer_destroy(bluetooth_icon_layer);
  layer_destroy(bluetooth_layer);
}
