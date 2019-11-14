#pragma once
#include <pebble.h>
#include "settings.h"
	
void background_settings_callback();
void background_panel_layer_update_callback(Layer *my_layer, GContext* ctx);
void background_init();
void background_deinit();