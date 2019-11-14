#pragma once
#include <pebble.h>
#include "settings.h"

void battery_settings_callback();
void battery_update(BatteryChargeState charge_state);
void battery_layer_update_callback(Layer *my_layer, GContext* ctx);
void battery_init();
void battery_deinit();
