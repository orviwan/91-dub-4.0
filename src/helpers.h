#pragma once
#include <pebble.h>
#include "_globals.h"

void duration_to_time(int duration_s, int *hours, int *minutes);
void format_commas(int n, char *out);
char *upcase(char *str);
uint8_t hex_to_num (char h);
//void animation_slide_in_cleanup();
void animation_slide_in(Layer *layer, int delay, direction_t direction);
TextLayer *text_layer_create_detailed(GRect frame, bool hidden, GColor background_color, GColor foreground_color, GTextAlignment alignment, GFont font);
GColor color_helper(GColor color, uint8_t inverted);

const char* get_gcolor_text(GColor m_color);
