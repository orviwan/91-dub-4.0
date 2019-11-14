#include <pebble.h>
#include "settings.h"
#include "helpers.h"

SettingsChangeCallback settings_callbacks[SETTINGS_CALLBACKS_COUNT] = {NULL};

Settings global_settings;
GColor colors[COLORS_NUM];
GColor colorsSet1[COLORS_NUM];
GColor colorsSet2[COLORS_NUM];
int8_t selectedSet= 0;

bool appStarted = false;
bool powerSaveEngaged = false;
bool settingsBusy = false;

AppTimer * delayed_save =NULL;

static void handle_tap(AccelAxisType axis, int32_t direction);

void settings_register_callback(SettingsChangeCallback callback, SettingsCallback callbackIdentity){
    settings_callbacks[callbackIdentity] = callback;
}
void settings_unregister_callback(SettingsCallback callbackIdentity){
    settings_callbacks[callbackIdentity] = NULL;
}

bool setting_is_set2(int8_t h, int8_t m){
  int8_t tested= h*2+1;
  if (m>=30) {
    tested++;
  }
  ////APP_LOG(APP_LOG_LEVEL_INFO, "%2d:%2d =>%d st:%d en:%d", h,m, tested, global_settings.SwitchStart, global_settings.SwitchEnd);
  if (global_settings.SwitchEnd > global_settings.SwitchStart) {
    return ( (tested >= global_settings.SwitchStart) && (tested < global_settings.SwitchEnd) );
  }
  else {
    return ( (tested >= global_settings.SwitchStart) || (tested < global_settings.SwitchEnd) );
  }
  return false; // safer to return some value in any case
}

bool setting_is_power_save(int8_t h, int8_t m){
  if (!global_settings.PowerSave) {
    return false;
  }
  int8_t tested= h*2+1;
  if (m>=30) {
    tested++;
  }
  if (global_settings.PS_End > global_settings.PS_Start) {
    return ( (tested >= global_settings.PS_Start) && (tested < global_settings.PS_End) );
  }
  else {
    return ( (tested >= global_settings.PS_Start) || (tested < global_settings.PS_End) );
  }
  return false; // safer to return some value in any case
}

void settings_process_tuple(Tuple *new_tuple) {
  int8_t cnt=0;
  cnt++;
  switch (new_tuple->key) {
    case HEALTH_KEY:
      global_settings.Health = new_tuple->value->uint8;
      break;
    case BLINK_KEY:
      global_settings.Blink = new_tuple->value->uint8;
      break;
    case INVERT_KEY:
      global_settings.Invert = new_tuple->value->uint8;
      break;
    case BLUETOOTHVIBE_KEY:
      global_settings.BluetoothVibe = new_tuple->value->uint8;
      break;
    case HOURLYVIBE_KEY:
      global_settings.HourlyVibe = new_tuple->value->uint8;
      break;
    case BRANDING_MASK_KEY:
      global_settings.BrandingMask = new_tuple->value->uint8;
      break;
    case BATTERY_HIDE_KEY:
      global_settings.BatteryHide = new_tuple->value->uint8;
      break;
    case SECONDS_KEY:
      global_settings.Seconds = new_tuple->value->uint8;
      break;
    case POWERSAVE_KEY:
      global_settings.PowerSave = new_tuple->value->uint8;
      break;
    case PS_START_KEY:
      global_settings.PS_Start = new_tuple->value->uint8;
      break;
    case PS_END_KEY:
      global_settings.PS_End = new_tuple->value->uint8;
      break;
    case SWITCHSET_KEY:
      global_settings.SwitchSet = new_tuple->value->uint8;
      break;
    case SWITCH_START_KEY:
      global_settings.SwitchStart = new_tuple->value->uint8;
      break;
    case SWITCH_END_KEY:
      global_settings.SwitchEnd = new_tuple->value->uint8;
      break;
    case SET_KEY:
      #ifdef PBL_COLOR
        ////APP_LOG(APP_LOG_LEVEL_INFO, "color set1 data: %s", new_tuple->value->cstring) ;
        cnt = strlen(new_tuple->value->cstring ) >>1;
        if (cnt>COLORS_NUM) {
          cnt=COLORS_NUM;
        }
        for (int i=0;i<cnt;i++) {
          colorsSet1[i].argb= (hex_to_num(new_tuple->value->cstring[i*2]) << 4) +
                                         hex_to_num(new_tuple->value->cstring[i*2+1]);
        }
      #endif
      break;
    case SET2_KEY:
      #ifdef PBL_COLOR
        ////APP_LOG(APP_LOG_LEVEL_INFO, "color set2 data: %s", new_tuple->value->cstring) ;
        cnt = strlen(new_tuple->value->cstring ) >>1;
        if (cnt>COLORS_NUM) {
          cnt=COLORS_NUM;
        }
        for (int i=0;i<cnt;i++) {
          colorsSet2[i].argb= (hex_to_num(new_tuple->value->cstring[i*2]) << 4) +
                                         hex_to_num(new_tuple->value->cstring[i*2+1]);
        }
      #endif
      break;
  }
}

void update_settings() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "update_settings()");

  if (!settingsBusy && appStarted) {
    settingsBusy = true;
    settings_callbacks[SETTINGS_CALLBACK_TIMEDIGITS]();
    for(int i = 0; i < SETTINGS_CALLBACKS_COUNT; i++) {
      if (settings_callbacks!= NULL && settings_callbacks[i] != NULL) {
        settings_callbacks[i]();
      }
    }
    if (global_settings.SwitchSet==2) {
      accel_tap_service_subscribe(handle_tap);
    }
    else {
      accel_tap_service_unsubscribe();
    }
    settingsBusy = false;
  }
}

void settings_inbox(DictionaryIterator *iter, void *context) {
  Tuple *t = dict_read_first(iter);
  if(t) {
    settings_process_tuple(t);
  }
  while(t != NULL) {
    t = dict_read_next(iter);
    if(t) {
      settings_process_tuple(t);
    }
  }
  if (selectedSet==0) {
    settings_load_colorSet1();
  }
  else {
    settings_load_colorSet2();
  }

  update_settings();

  //Delayed setting save instead of in exit. The delay allow screen update before save
  delayed_save= app_timer_register(100, settings_save, NULL);
}


void settings_default_values() {
  global_settings.Health = 0;
  global_settings.Blink = 1;
  global_settings.Invert = 0;
  global_settings.BluetoothVibe = 1;
  global_settings.HourlyVibe = 1;
  global_settings.BrandingMask = 0;
  global_settings.BatteryHide = 0;
  global_settings.Seconds = 0;
  global_settings.PowerSave = 0;
  global_settings.PS_Start = 47;   //23:00
  global_settings.PS_End = 15;     //07:00
  global_settings.SwitchSet = 0;
  global_settings.SwitchStart = 47;//23:00
  global_settings.SwitchEnd = 15;  //07:00
  colors[c_bg1] = GColorWhite;
  colors[c_bg2] = GColorBlack;
  colors[c_bg3] = GColorWhite;
  colors[c_bg4] = GColorBlack;

  colors[c_bi1] = GColorBlack;
  colors[c_bi2] = PBL_IF_COLOR_ELSE(GColorOrange, GColorBlack);
  colors[c_bi3] = PBL_IF_COLOR_ELSE(GColorRed, GColorBlack);
  colors[c_bi4] = PBL_IF_COLOR_ELSE(GColorRed, GColorBlack);

  colors[c_bl1] = PBL_IF_COLOR_ELSE(GColorDukeBlue, GColorWhite);
  colors[c_bl2] = PBL_IF_COLOR_ELSE(GColorLightGray, GColorBlack);
  colors[c_bl3] = PBL_IF_COLOR_ELSE(GColorRed, GColorWhite);
  colors[c_bl4] = GColorWhite;

  colors[c_d1]  = GColorWhite;
  colors[c_d2]  = GColorWhite;
  colors[c_d3]  = GColorWhite;
  colors[c_d4]  = GColorBlack;
  colors[c_d5]  = GColorWhite;
  colors[c_d6]  = GColorWhite;
  colors[c_d7]  = GColorWhite;
  colors[c_d8]  = GColorWhite;
  colors[c_d9]  = GColorWhite;

  colors[c_t1]  = GColorBlack;
  colors[c_t2]  = GColorBlack;
  colors[c_t3]  = GColorWhite;
  colors[c_t4]  = GColorBlack;

  memcpy(colorsSet1,colors,COLORS_NUM);
  memcpy(colorsSet2,colors,COLORS_NUM);
}

void settings_load_colorSet1() {
  memcpy(colors, colorsSet1, COLORS_NUM);
  selectedSet = 0;
}
void settings_load_colorSet2() {
  memcpy(colors, colorsSet2, COLORS_NUM);
  selectedSet = 1;
}

static void handle_tap(AccelAxisType axis, int32_t direction) {
  if (selectedSet==0) {
    settings_load_colorSet2();
  }
  else {
    settings_load_colorSet1();
  }
  update_settings();
}

void timed_colorset(int8_t h, int8_t m){
  if (global_settings.SwitchSet==1){
    bool isSet2 = setting_is_set2(h, m);
    if ((selectedSet == 0) && isSet2) {
      settings_load_colorSet2();
        update_settings();
    }
    else if ((selectedSet == 1) && !isSet2) {
      settings_load_colorSet1();
        update_settings();
    }
  }
}

void settings_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "settings_init()");

  settings_default_values();
  //persist_delete(1337);
  if(persist_exists(SETTINGS_KEY)) {
    persist_read_data(SETTINGS_KEY, &global_settings, sizeof(global_settings));
  }
  #ifdef PBL_COLOR
    if(persist_exists(COLORSET1_KEY)) {
      persist_read_data(COLORSET1_KEY, &colorsSet1, sizeof(colorsSet1));
    }
    if(persist_exists(COLORSET2_KEY)) {
      persist_read_data(COLORSET2_KEY, &colorsSet2, sizeof(colorsSet2));
    }
  #endif

  //set configuration according to time the CLOCK
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  settings_load_colorSet1();

  timed_colorset(tick_time->tm_hour,tick_time->tm_min);
  powerSaveEngaged = false;
  if (global_settings.PowerSave==1) {
    powerSaveEngaged = setting_is_power_save(tick_time->tm_hour,tick_time->tm_min);
  }

  app_message_register_inbox_received(settings_inbox);
  app_message_open(512, 512);


  if (global_settings.SwitchSet==2) {
    accel_tap_service_subscribe(handle_tap);
  }
}

void settings_save(void *data) {
  delayed_save =NULL;
  persist_write_data(SETTINGS_KEY, &global_settings, sizeof(global_settings));
  #ifdef PBL_COLOR
  persist_write_data(COLORSET1_KEY, &colorsSet1, sizeof(colors));
  persist_write_data(COLORSET2_KEY, &colorsSet2, sizeof(colors));
  #endif
}

void settings_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "settings_deinit()");

  accel_tap_service_unsubscribe();
  // no need to save on exit unless the save after setting update did not happen yet
  if (delayed_save !=NULL) {
    app_timer_cancel(delayed_save);
    settings_save(NULL);
  }
}
