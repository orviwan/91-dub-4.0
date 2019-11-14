#include <pebble.h>
#include "_globals.h"
#include "health.h"
#include "settings.h"
#include "helpers.h"
#include "window.h"
 #include "fonts.h"

static bool health_enabled = false;

#ifdef PBL_HEALTH

static TextLayer *health_text_layer;
static Layer *health_layer, *health_foot_layer, *health_foot2_layer, *health_zee_layer;

static GPath *foot_path_ptr = NULL;
static GPathInfo FOOT_PATH_INFO = {
  .num_points = 13,
  .points = (GPoint []) {{0,1}, {1,1}, {1,0}, {2,0}, {2,1}, {3,1}, {3,3}, {2,3}, {2,5}, {1,5}, {1,4}, {0,4}, {0,1}}
};
static GPath *heel_path_ptr = NULL;
static GPathInfo HEEL_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{1,7}, {2,7}, {2,8}, {1,8}}
};
static GPath *zee1_path_ptr = NULL;
static GPathInfo ZEE1_PATH_INFO = {
  .num_points = 6,
  .points = (GPoint []) {{0,2}, {2,2}, {2,3}, {0,5}, {0,6}, {2,6}}
};
static GPath *zee2_path_ptr = NULL;
static GPathInfo ZEE2_PATH_INFO = {
  .num_points = 6,
  .points = (GPoint []) {{3,1}, {6,1}, {6,2}, {3,5}, {3,6}, {6,6}}
};
static GPath *zee3_path_ptr = NULL;
static GPathInfo ZEE3_PATH_INFO = {
  .num_points = 6,
  .points = (GPoint []) {{7,0}, {11,0}, {11,1}, {7,5}, {7,6}, {11,6}}
};



static HealthValue s_sleep, s_deep_sleep, s_steps, s_active, s_distance;

void health_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_t2], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_t2], global_settings.Invert));
  if(s_steps < HEALTH_STEP_MIN) {
    //zzz
    gpath_draw_outline_open(ctx, zee1_path_ptr);
    gpath_draw_outline_open(ctx, zee2_path_ptr);
    gpath_draw_outline_open(ctx, zee3_path_ptr);
  } else {
    //steps
    gpath_draw_filled(ctx, foot_path_ptr);
    gpath_draw_outline(ctx, foot_path_ptr);
    gpath_draw_filled(ctx, heel_path_ptr);
    gpath_draw_outline(ctx, heel_path_ptr);
  }

}

void health_settings_callback() {
  health_deinit();

  #if defined (PBL_HEALTH)
    health_init();
  #endif
}

void health_update() {
  static char str[20], str2[20];
  if(s_steps < HEALTH_STEP_MIN) {
    //zzz
    int hours = 0, minutes = 0;
    duration_to_time(s_sleep, &hours, &minutes);

    if(hours>0) {
      snprintf(str2, sizeof(str2), "%dH'%dM", hours, minutes);
    } else {
      snprintf(str2, sizeof(str2), "%dM", minutes);
    }

    #if defined (PBL_ROUND)
      GRect r = layer_get_frame(health_layer);
      if(hours == 0 && minutes < 10) {
        r.origin.x = 76;
      } else if(hours == 0) {
        r.origin.x = 73;
      } else if(hours < 10 && minutes < 10) {
        r.origin.x = 67;
      } else {
        r.origin.x = 62;
      }
      layer_set_frame(health_layer, r);
    #endif

    layer_set_hidden(health_zee_layer, false);
    layer_set_hidden(health_foot_layer, true);
    layer_set_hidden(health_foot2_layer, true);
  } else {
    //steps
    format_commas(s_steps, str);
    snprintf(str2, sizeof(str2), "%s", str);

    #if defined (PBL_ROUND)
      GRect r = layer_get_frame(health_layer);
      if(s_steps < 1000) {
        r.origin.x = 71;
      } else if(s_steps < 10000) {
        r.origin.x = 66;
      } else {
        r.origin.x = 61;
      }
      layer_set_frame(health_layer, r);
    #endif

    layer_set_hidden(health_zee_layer, true);
    layer_set_hidden(health_foot_layer, false);
    layer_set_hidden(health_foot2_layer, false);
  }
  text_layer_set_text(health_text_layer, str2);
}

void health_handler(HealthEventType event, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "health_handler");
  if (event != HealthEventSleepUpdate) {
    s_steps = health_service_sum_today(HealthMetricStepCount);
		//s_distance = health_service_sum_today(HealthMetricStepCount);
		//s_active = health_service_sum_today(HealthMetricActiveSeconds);
  }
  if (event != HealthEventMovementUpdate ) {
    s_sleep = health_service_sum_today(HealthMetricSleepSeconds);
    //s_deep_sleep = health_service_sum_today(HealthMetricSleepRestfulSeconds);
  }
  health_update();

}

void health_init() {

  if(!global_settings.Health) {
    return;
  }

  health_layer = layer_create(HEALTH_LAYER);
  layer_add_child(my_window_layer, health_layer);

  health_text_layer = text_layer_create_detailed(HEALTH_TEXT_LAYER, false,
                                GColorClear, color_helper(colors[c_t2], global_settings.Invert),
                                GTextAlignmentLeft, font_tiny);
  layer_add_child(health_layer, text_layer_get_layer(health_text_layer));

  foot_path_ptr = gpath_create(&FOOT_PATH_INFO);
  heel_path_ptr = gpath_create(&HEEL_PATH_INFO);
  zee1_path_ptr = gpath_create(&ZEE1_PATH_INFO);
  zee2_path_ptr = gpath_create(&ZEE2_PATH_INFO);
  zee3_path_ptr = gpath_create(&ZEE3_PATH_INFO);

  health_zee_layer = layer_create(GRect(0,3,14,7));
  layer_set_update_proc(health_zee_layer, health_icon_layer_update_callback);
  layer_add_child(health_layer, health_zee_layer);

  health_foot_layer = layer_create(GRect(3,0,9,12));
  layer_set_update_proc(health_foot_layer, health_icon_layer_update_callback);
  layer_add_child(health_layer, health_foot_layer);

  health_foot2_layer = layer_create(GRect(8,3,9,12));
  layer_set_update_proc(health_foot2_layer, health_icon_layer_update_callback);
  layer_add_child(health_layer, health_foot2_layer);

  health_service_events_subscribe(health_handler, NULL);
	health_handler(HealthEventMovementUpdate, NULL);
	health_handler(HealthEventSleepUpdate, NULL);

  settings_register_callback(health_settings_callback, SETTINGS_CALLBACK_HEALTH);

  health_enabled = true;
}

void health_deinit() {

  if(!health_enabled) {
      return;
  }
  gpath_destroy(foot_path_ptr);
  foot_path_ptr = NULL;

  gpath_destroy(heel_path_ptr);
  heel_path_ptr = NULL;

  layer_destroy(health_foot_layer);
  layer_destroy(health_foot2_layer);
  layer_destroy(health_zee_layer);
  text_layer_destroy(health_text_layer);
  layer_destroy(health_layer);

  health_enabled = false;

}

#endif
