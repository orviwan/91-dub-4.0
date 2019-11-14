#pragma once
#include <pebble.h>

extern GFont font_big, font_small, font_tiny;

void fonts_settings_callback();
void fonts_init();
void fonts_deinit();