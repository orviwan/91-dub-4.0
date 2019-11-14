#include <pebble.h>
#include "fonts.h"
#include "settings.h"

GFont font_big, font_small, font_tiny;

void fonts_settings_callback() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "fonts_settings_callback()");

  fonts_unload_custom_font(font_big);
  int32_t font_res= (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DIGITALE_56 : RESOURCE_ID_FONT_DIGITALE_71;
  //if (global_settings.Seconds) //ToDo: update font size for seconds mode.
  font_big = fonts_load_custom_font(resource_get_handle(font_res));
}
void fonts_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "fonts_init()");

  int32_t font_res= (global_settings.Seconds && !powerSaveEngaged) ? RESOURCE_ID_FONT_DIGITALE_56 : RESOURCE_ID_FONT_DIGITALE_71;
  font_big = fonts_load_custom_font(resource_get_handle(font_res));
  font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGITALE_22));
  font_tiny = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LUCIDIA_10)); 
  settings_register_callback(fonts_settings_callback, SETTINGS_CALLBACK_FONT);

}
void fonts_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "fonts_deinit()");
  
  fonts_unload_custom_font(font_big);
  fonts_unload_custom_font(font_small);
  fonts_unload_custom_font(font_tiny);
}