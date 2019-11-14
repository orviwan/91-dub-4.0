#include <pebble.h>
#include "_globals.h"
#include "background.h"
#include "helpers.h"
#include "settings.h"
#include "window.h"

static Layer *background_layer, *background_panel_layer;

/*
bg1 Centre Panel Outer Stroke
bg2 Centre Panel Stroke
bg3 Centre Panel
bg4 Background Color
*/
void background_settings_callback() {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "background_settings_callback()");
	window_set_background_color(my_window, color_helper(colors[c_bg4], global_settings.Invert));
	layer_mark_dirty(background_layer);
}
void background_layer_update_callback(Layer *my_layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, color_helper(colors[c_bg4], global_settings.Invert));
  graphics_fill_rect(ctx, layer_get_bounds(my_layer), 0, GCornerNone);
}

void background_panel_layer_update_callback(Layer *my_layer, GContext* ctx) {
	GRect bounds = layer_get_bounds(my_layer);
  GPoint center = grect_center_point(&bounds);

	graphics_context_set_fill_color(ctx, color_helper(colors[c_bg1], global_settings.Invert));
	#if defined (PBL_RECT)
		graphics_fill_rect(ctx, BACKGROUND_PANEL_OUTER, 8, GCornersAll);
	#else
		//graphics_fill_radial(ctx, FULLSCREEN, GOvalScaleModeFillCircle, 65, 0, 360);
		graphics_fill_circle(ctx, center, 82);
	#endif

	graphics_context_set_fill_color(ctx, color_helper(colors[c_bg2], global_settings.Invert));
	#if defined (PBL_RECT)
		graphics_fill_rect(ctx, BACKGROUND_PANEL_MIDDLE, 8, GCornersAll);
	#else
		//graphics_fill_radial(ctx, BACKGROUND_PANEL_MIDDLE, GOvalScaleModeFillCircle, 65, 0, 360);
		graphics_fill_circle(ctx, center, 80);
	#endif

	graphics_context_set_fill_color(ctx, color_helper(colors[c_bg3], global_settings.Invert));
	#if defined (PBL_RECT)
		graphics_fill_rect(ctx, BACKGROUND_PANEL_INNER, 7, GCornersAll);
	#else
		//graphics_fill_radial(ctx, BACKGROUND_PANEL_INNER, GOvalScaleModeFillCircle, 65, 0, 360);
		graphics_fill_circle(ctx, center, 78);
	#endif
}

void background_init() {

	background_layer = layer_create(FULLSCREEN);
	layer_add_child(my_window_layer, background_layer);

	background_panel_layer = layer_create(BACKGROUND_PANEL);
	layer_add_child(background_layer, background_panel_layer);

	layer_set_update_proc(background_layer, background_layer_update_callback);
	layer_set_update_proc(background_panel_layer, background_panel_layer_update_callback);

	settings_register_callback(background_settings_callback, SETTINGS_CALLBACK_BACKGROUND);
}

void background_deinit() {

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "background_deinit()");

	layer_remove_from_parent(background_panel_layer);
	layer_remove_from_parent(background_layer);

	layer_destroy(background_panel_layer);
	layer_destroy(background_layer);
}
