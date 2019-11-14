#include <pebble.h>
#include "main.h"
#include "helpers.h"
#include "settings.h"
#include "bluetooth.h"
#include "health.h"
#include "battery.h"
#include "timedigits.h"
#include "decorations.h"
#include "fonts.h"
#include "window.h"
#include "background.h"
#include "unobstructed.h"

AppTimer * started_timer =NULL;

void reload(void){
  background_deinit();
  fonts_deinit();
  timedigits_deinit();
  battery_deinit();
  bluetooth_deinit();
  decorations_deinit();
  unobstructed_deinit();
  fonts_init();
  background_init();
  decorations_init();
  timedigits_init();
  battery_init();
  #if defined (PBL_HEALTH)
    health_init();
  #endif
  bluetooth_init();
  appStarted = true;
}

void app_sarted_callback(void *data) {
  started_timer =NULL;
  appStarted = true;
}
void handle_init(void) {
  appStarted = false;

  setlocale(LC_ALL, "");

  settings_init();
  window_init();
  fonts_init();
  background_init();
  decorations_init();
  unobstructed_init();
  timedigits_init();
  if (!powerSaveEngaged) {
    battery_init();
    #if defined (PBL_HEALTH)
      health_init();
    #endif
    bluetooth_init();
  }

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "memfree %d", heap_bytes_free());

  //delay started flag. Prevent settings vibe during first few seconds (HOUR_UNIT included in settings update)
  started_timer = app_timer_register(4000, app_sarted_callback, NULL);
}

void handle_deinit(void) {
  if (started_timer!=NULL) {
    app_timer_cancel(started_timer);
  }
  settings_deinit();
  background_deinit();
  fonts_deinit();
  timedigits_deinit();
  if (!powerSaveEngaged) {
    battery_deinit();
    #if defined (PBL_HEALTH)
      health_deinit();
    #endif
    bluetooth_deinit();
  }
  decorations_deinit();
  unobstructed_deinit();
  window_deinit();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
