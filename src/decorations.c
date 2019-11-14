#include <pebble.h>
#include "decorations.h"
#include "_globals.h"
#include "helpers.h"
#include "settings.h"
#include "fonts.h"
#include "window.h"

static Layer *decorations_layer, *wr_outer_layer, *button_back_icon_layer, *button_next_icon_layer, *button_prev_icon_layer;
static TextLayer *water_layer, *resist_layer, *button_back_layer, *button_next_layer, *button_prev_layer;
static BitmapLayer *logo_layer;
static GBitmap *logo_image;

static GPath *arrow_left_path_ptr = NULL;
static GPathInfo ARROW_LEFT_PATH_INFO = {
  .num_points = 5,
  .points = (GPoint []) {{0,2}, {4,0}, {4,5}, {0,3}, {0,2}}
};

static GPath *arrow_right_path_ptr = NULL;
static GPathInfo ARROW_RIGHT_PATH_INFO = {
  .num_points = 5,
  .points = (GPoint []) {{0,0}, {4,2}, {4,3}, {0,5}, {0,0}}
};

static GPath *wr_outer_path_ptr = NULL;
static GPathInfo WR_OUTER_PATH_INFO = {
  .num_points = 9,
  .points = (GPoint []) {{0,2}, {2,0}, {39,0}, {41,2}, {41,9}, {35,15}, {5,15}, {0,10}, {0,2}}
};

static GPath *wr_w_path_ptr = NULL;
static GPathInfo WR_W_PATH_INFO = {
  .num_points = 39,
  .points = (GPoint []) {{ 7, 2},{10, 2},{10, 9},{12, 9},{12, 6},{13, 6},
                         {13, 4},{14, 4},{14, 2},{16, 2},{16, 9},{18, 9},
                         {18, 7},{19, 7},{19, 4},{20, 4},{20, 2},{22, 2},
                         {22, 3},{21, 3},{21, 6},{20, 6},{20, 8},{19, 8},
                         {19,11},{18,11},{18,13},{15,13},{15, 6},
                         {13, 6},{13, 9},{12, 9},{12,11},{11,11},{11,13},
                         { 8,13},{ 8, 8},{ 7, 8},{ 7, 2}}
};

static GPath *wr_r_path_ptr = NULL;
static GPathInfo WR_R_PATH_INFO = {
  .num_points = 27,
  .points = (GPoint []) {{24, 2},{31, 2},{31, 3},{32, 3},{32, 4},{33, 4},
                         {33, 7},{32, 7},{32, 8},{31, 8},{31,10},{32,10},
                         {32,13},{29,13},{29,10},{28,10},{28, 9},{26, 9},
                         {26,10},{25,10},{25,13},{22,13},
                         {22,11},{23,11},{23, 5},{24, 5},{24, 2}}
};

/*
d1  Horizontal Line Top
d2  Horizontal Line Bottom
d3  Water Resist Box Stroke
d4  Water Resist Box Fill
d5  WR Letters
d6  Water Resist Text
d7  Button Labels
d8  Button Icons
d9  Branding Text
*/

void decorations_settings_callback() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "decorations_settings_callback()");

  text_layer_set_text_color(water_layer, color_helper(colors[c_d6], global_settings.Invert));
  text_layer_set_text_color(resist_layer, color_helper(colors[c_d6], global_settings.Invert));

  text_layer_set_text_color(button_back_layer, color_helper(colors[c_d7], global_settings.Invert));
  text_layer_set_text_color(button_next_layer, color_helper(colors[c_d7], global_settings.Invert));
  text_layer_set_text_color(button_prev_layer, color_helper(colors[c_d7], global_settings.Invert));

  #ifdef PBL_COLOR
      GColor * xcolors = gbitmap_get_palette(logo_image);
      xcolors[0].argb = color_helper(colors[c_bg4], global_settings.Invert).argb;
      xcolors[1].argb = color_helper(colors[c_d9], global_settings.Invert).argb;
  #endif

  if(global_settings.BrandingMask) {
    layer_set_hidden(bitmap_layer_get_layer(logo_layer), true);
  }
  else {
    layer_set_hidden(bitmap_layer_get_layer(logo_layer), false);
  }

  layer_mark_dirty(decorations_layer);
}

void back_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Arrow BACK
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  gpath_draw_filled(ctx, arrow_left_path_ptr);
  gpath_draw_outline(ctx, arrow_left_path_ptr);
}


void next_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Arrow NEXT
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  gpath_draw_filled(ctx, arrow_right_path_ptr);
  gpath_draw_outline(ctx, arrow_right_path_ptr);
}


void prev_icon_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Arrow PREV
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d8], global_settings.Invert));
  gpath_draw_filled(ctx, arrow_right_path_ptr);
  gpath_draw_outline(ctx, arrow_right_path_ptr);
}

void decorations_layer_update_callback(Layer *my_layer, GContext* ctx) {
  //Line Top
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d1], global_settings.Invert));
  graphics_draw_line(ctx, DECORATIONS_LINE_TOP_START, DECORATIONS_LINE_TOP_END);

  //Line Bottom
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d2], global_settings.Invert));
  graphics_draw_line(ctx, DECORATIONS_LINE_BOTTOM_START, DECORATIONS_LINE_BOTTOM_END);
}

void decorations_wr_outer_layer_update_callback(Layer *my_layer, GContext* ctx) {

  //Box surrounding WR
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d3], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d4], global_settings.Invert));
  gpath_draw_filled(ctx, wr_outer_path_ptr);
  gpath_draw_outline(ctx, wr_outer_path_ptr);

  //W and R
  graphics_context_set_stroke_color(ctx, color_helper(colors[c_d5], global_settings.Invert));
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d5], global_settings.Invert));
  gpath_draw_filled(ctx, wr_w_path_ptr);
  gpath_draw_outline(ctx, wr_w_path_ptr);
  gpath_draw_filled(ctx, wr_r_path_ptr);
  gpath_draw_outline(ctx, wr_r_path_ptr);

  //Hole in the R
  graphics_context_set_fill_color(ctx, color_helper(colors[c_d4], global_settings.Invert));
  graphics_fill_rect(ctx, GRect(27,5,3,2), 0, GCornerNone);

}

void decorations_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "decorations_init()");

  decorations_layer = layer_create(FULLSCREEN);
  //layer_set_hidden(decorations_layer, true);
  layer_add_child(my_window_layer, decorations_layer);
  layer_set_update_proc(decorations_layer, decorations_layer_update_callback);


  button_back_icon_layer = layer_create(DECORATIONS_BUTTON_BACK_ICON);
  layer_set_update_proc(button_back_icon_layer, back_icon_layer_update_callback);
  layer_add_child(decorations_layer, button_back_icon_layer);

  button_next_icon_layer = layer_create(DECORATIONS_BUTTON_NEXT_ICON);
  layer_set_update_proc(button_next_icon_layer, next_icon_layer_update_callback);
  layer_add_child(decorations_layer, button_next_icon_layer);

  button_prev_icon_layer = layer_create(DECORATIONS_BUTTON_PREV_ICON);
  layer_set_update_proc(button_prev_icon_layer, prev_icon_layer_update_callback);
  layer_add_child(decorations_layer, button_prev_icon_layer);

  arrow_left_path_ptr = gpath_create(&ARROW_LEFT_PATH_INFO);
  arrow_right_path_ptr = gpath_create(&ARROW_RIGHT_PATH_INFO);

  wr_outer_path_ptr = gpath_create(&WR_OUTER_PATH_INFO);
  wr_w_path_ptr = gpath_create(&WR_W_PATH_INFO);
  wr_r_path_ptr = gpath_create(&WR_R_PATH_INFO);

  wr_outer_layer = layer_create(DECORATIONS_WR_OUTER);
  //layer_set_hidden(wr_outer_layer, true);
  layer_set_update_proc(wr_outer_layer, decorations_wr_outer_layer_update_callback);
  layer_add_child(decorations_layer, wr_outer_layer);

  // WATER LABEL
  water_layer = text_layer_create(DECORATIONS_WR_WATER);
  text_layer_set_text_alignment(water_layer, GTextAlignmentRight);
  text_layer_set_text(water_layer, "WATER");
  text_layer_set_background_color(water_layer, GColorClear);
  text_layer_set_text_color(water_layer, color_helper(colors[c_d6], global_settings.Invert));
  text_layer_set_font(water_layer, font_tiny);
  layer_add_child(decorations_layer, text_layer_get_layer(water_layer));

  // RESIST LABEL
  resist_layer = text_layer_create(DECORATIONS_WR_RESIST);
  text_layer_set_text_alignment(resist_layer, GTextAlignmentLeft);
  text_layer_set_text(resist_layer, "RESIST");
  text_layer_set_background_color(resist_layer, GColorClear);
  text_layer_set_text_color(resist_layer, color_helper(colors[c_d6], global_settings.Invert));
  text_layer_set_font(resist_layer, font_tiny);
  layer_add_child(decorations_layer, text_layer_get_layer(resist_layer));

  // BACK BUTTON LABEL
  button_back_layer = text_layer_create_detailed(DECORATIONS_BUTTON_BACK_LABEL, false, GColorClear
                                                 , color_helper(colors[c_d7], global_settings.Invert),
                                                 GTextAlignmentLeft, font_tiny);
  text_layer_set_text(button_back_layer, "LIGHT");
  layer_add_child(decorations_layer, text_layer_get_layer(button_back_layer));


  // NEXT BUTTON LABEL
  button_next_layer = text_layer_create_detailed(DECORATIONS_BUTTON_NEXT_LABEL, false, GColorClear
                                                 , color_helper(colors[c_d7], global_settings.Invert),
                                                 GTextAlignmentRight, font_tiny);
  text_layer_set_text(button_next_layer, "NEXT");
  layer_add_child(decorations_layer, text_layer_get_layer(button_next_layer));


  // PREV BUTTON LABEL
  button_prev_layer = text_layer_create_detailed(DECORATIONS_BUTTON_PREV_LABEL, false, GColorClear,
                                                  color_helper(colors[c_d7], global_settings.Invert),
                                                  GTextAlignmentRight, font_tiny);
  text_layer_set_text(button_prev_layer, "PREV");
  layer_add_child(decorations_layer, text_layer_get_layer(button_prev_layer));

  // BRANDING LABEL
  logo_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOGO2);
  logo_layer = bitmap_layer_create(GRect(11, 2, 119, 15));
  #ifdef PBL_COLOR
    GColor * xcolors = gbitmap_get_palette(logo_image);
    xcolors[0].argb = color_helper(colors[c_bg4], global_settings.Invert).argb;
    xcolors[1].argb = color_helper(colors[c_d9], global_settings.Invert).argb;
  #endif
  bitmap_layer_set_bitmap(logo_layer, logo_image);
  layer_add_child(decorations_layer, bitmap_layer_get_layer(logo_layer));

  settings_register_callback(decorations_settings_callback, SETTINGS_CALLBACK_DECORATIONS);


  #if defined (PBL_ROUND)
    layer_set_hidden(button_back_icon_layer, true);
    layer_set_hidden(button_next_icon_layer, true);
    layer_set_hidden(button_prev_icon_layer, true);
    layer_set_hidden(text_layer_get_layer(button_back_layer), true);
    layer_set_hidden(text_layer_get_layer(button_next_layer), true);
    layer_set_hidden(text_layer_get_layer(button_prev_layer), true);

    layer_set_hidden(bitmap_layer_get_layer(logo_layer), true);

    layer_set_hidden(text_layer_get_layer(water_layer), true);
    layer_set_hidden(text_layer_get_layer(resist_layer), true);

  #endif

  //animation_slide_in(decorations_layer, 700, RIGHT);
  //animation_slide_in(wr_outer_layer, 700, UP);
}

void decorations_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "decorations_init()");

  gpath_destroy(wr_outer_path_ptr);
  gpath_destroy(wr_w_path_ptr);
  gpath_destroy(wr_r_path_ptr);
  gpath_destroy(arrow_left_path_ptr);
  gpath_destroy(arrow_right_path_ptr);

  wr_outer_path_ptr = NULL;
  wr_w_path_ptr = NULL;
  wr_r_path_ptr = NULL;
  arrow_left_path_ptr = NULL;
  arrow_right_path_ptr = NULL;

  layer_remove_from_parent(bitmap_layer_get_layer(logo_layer));
  gbitmap_destroy(logo_image);
  logo_image = NULL;
  bitmap_layer_destroy(logo_layer);

  layer_remove_from_parent(text_layer_get_layer(water_layer));
  layer_remove_from_parent(text_layer_get_layer(resist_layer));
  layer_remove_from_parent(text_layer_get_layer(button_back_layer));
  layer_remove_from_parent(text_layer_get_layer(button_next_layer));
  layer_remove_from_parent(text_layer_get_layer(button_prev_layer));

  layer_remove_from_parent(button_back_icon_layer);
  layer_remove_from_parent(button_next_icon_layer);
  layer_remove_from_parent(button_prev_icon_layer);
  layer_remove_from_parent(wr_outer_layer);
  layer_remove_from_parent(decorations_layer);

  text_layer_destroy(water_layer);
  text_layer_destroy(resist_layer);
  text_layer_destroy(button_back_layer);
  text_layer_destroy(button_next_layer);
  text_layer_destroy(button_prev_layer);

  layer_destroy(button_back_icon_layer);
  layer_destroy(button_next_icon_layer);
  layer_destroy(button_prev_icon_layer);
  layer_destroy(wr_outer_layer);
  layer_destroy(decorations_layer);
}

void decorations_toggle(bool is_obstructed) {
  if(is_obstructed) {
    layer_set_hidden(decorations_layer, true);
  } else {
    layer_set_hidden(decorations_layer, false);
  }
}
