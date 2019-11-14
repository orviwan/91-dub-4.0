#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1336
#define COLORSET1_KEY 1341
#define COLORSET2_KEY 1342

#define SETTINGS_CALLBACKS_COUNT 7
#define COLORS_NUM 25

typedef enum SettingsCallback {
  SETTINGS_CALLBACK_FONT = 0,
  SETTINGS_CALLBACK_BATTERY = 1,
  SETTINGS_CALLBACK_BLUETOOTH = 2,
  SETTINGS_CALLBACK_DECORATIONS = 3,
  SETTINGS_CALLBACK_TIMEDIGITS = 4,
  SETTINGS_CALLBACK_BACKGROUND = 5,
  SETTINGS_CALLBACK_HEALTH = 6
} SettingsCallback;

enum {
  c_bg1, c_bg2, c_bg3, c_bg4, //background
  c_bi1, c_bi2, c_bi3, c_bi4, //battery
  c_bl1, c_bl2, c_bl3, c_bl4, //bluetooth
  c_d1, c_d2, c_d3, c_d4, c_d5, c_d6, c_d7, c_d8, c_d9, //decorations
  c_t1, c_t2, c_t3, c_t4 //time digits
 };

typedef struct Settings {
  uint8_t Health;
  uint8_t Blink;
  uint8_t Invert;
  uint8_t BluetoothVibe;
  uint8_t HourlyVibe;
  uint8_t BrandingMask;
  uint8_t BatteryHide;
  uint8_t Seconds;
  uint8_t PowerSave;
  uint8_t PS_Start;
  uint8_t PS_End;
  uint8_t SwitchSet; //0 inactive, 1-time, 2-tap
  uint8_t SwitchStart;
  uint8_t SwitchEnd;
} __attribute__((__packed__)) Settings;

extern Settings global_settings;
extern GColor colors[COLORS_NUM];
extern int8_t selectedSet;

typedef void (*SettingsChangeCallback)();

enum {
  BLINK_KEY, INVERT_KEY, BLUETOOTHVIBE_KEY, HOURLYVIBE_KEY,
  BRANDING_MASK_KEY, BATTERY_HIDE_KEY, SECONDS_KEY,
  POWERSAVE_KEY, PS_START_KEY, PS_END_KEY,
  SWITCHSET_KEY, SWITCH_START_KEY, SWITCH_END_KEY, HEALTH_KEY
};

#define SET_KEY  200
#define SET1_KEY 201
#define SET2_KEY 202


enum {
  BLINK_OFF = 0,
  BLINK_ON = 1,
  BLINK_DOUBLE_RATE = 2,
};


extern bool appStarted;
extern bool powerSaveEngaged;

bool setting_is_power_save(int8_t h, int8_t m);
bool setting_is_set2(int8_t h, int8_t m);
void update_settings();
void timed_colorset(int8_t h, int8_t m);
void settings_register_callback(SettingsChangeCallback callback, SettingsCallback callbackIdentity);
void settings_unregister_callback(SettingsCallback callbackIdentity);
void settings_process_tuple(Tuple *new_tuple);
void settings_inbox(DictionaryIterator *iter, void *context);
//Settings settings_get();
void settings_default_values();
void settings_load_colorSet1();
void settings_load_colorSet2();
void settings_save(void *data);
void settings_init();
void settings_deinit();
