#pragma once
#include <pebble.h>
#include "settings.h"

void prv_unobstructed_will_change(GRect final_unobstructed_screen_area, void *context);
void prv_unobstructed_did_change(void *context);
void prv_unobstructed_change(AnimationProgress progress, void *context);
void unobstructed_init();
void unobstructed_deinit();
