#pragma once
#include <pebble.h>
#include "settings.h"

void bluetooth_settings_callback();
void bluetooth_circle_layer_update_callback(Layer *my_layer, GContext* ctx);
void bluetooth_layer_update_callback(Layer *my_layer, GContext* ctx);
void bluetooth_icon_toggle(uint8_t BluetoothVibe);
void bluetooth_connection_callback(bool connected);
void bluetooth_init();
void bluetooth_deinit();
