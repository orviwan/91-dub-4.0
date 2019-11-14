#pragma once
#include <pebble.h>
#include "settings.h"

void timedigits_settings_callback();
void handle_tick(struct tm *tick_time, TimeUnits units_changed);
void timedigits_foreground_color(GColor foreground);
void timedigits_background_color(GColor background);
void timedigits_color(GColor foreground, GColor background);
TextLayer *timedigits_init_big_digit(int d);
void timedigits_init();
void timedigits_deinit();