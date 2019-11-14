#include <pebble.h>
#include "timedigits.h"
#include "_globals.h"
#include "settings.h"
#include "helpers.h"
#include "window.h"
#include "fonts.h"
#include "bluetooth.h"
#include "battery.h"

enum {t_dig1, t_dig2, t_sep, t_dig3, t_dig4, t_dig5, t_dig6,
      t_shadow_dig1, t_shadow_dig2, t_shadow_sep, t_shadow_dig3, t_shadow_dig4, t_shadow_dig5, t_shadow_dig6,
      t_ampm, t_date};
#define tl_last t_date
static TextLayer * t_layer[tl_last+1] = {NULL};
static Layer *clock_layer, *center_layer;

static PropertyAnimation *separator_animation;
static char time_text[] = "0000";

typedef struct  __attribute__((__packed__)){
  uint8_t left;
  uint8_t top;
  uint8_t width;
  uint8_t hight;
  char*   text;
  uint8_t color;
  uint8_t font;
} digit_data_t;

enum {f_big, f_sec};
digit_data_t digit_data[]={
  {TIMEDIGITS_DIGIT1,   TIMEDIGITS_OFFSET_TOP, TIMEDIGITS_WIDTH, TIMEDIGITS_HEIGHT, "8", c_t3, f_big },
  {TIMEDIGITS_DIGIT2,   TIMEDIGITS_OFFSET_TOP, TIMEDIGITS_WIDTH, TIMEDIGITS_HEIGHT, "8", c_t3, f_big },
  {TIMEDIGITS_SEPARATOR,TIMEDIGITS_OFFSET_TOP, TIMEDIGITS_WIDTH, TIMEDIGITS_HEIGHT, ":", c_t3, f_big },
  {TIMEDIGITS_DIGIT3,   TIMEDIGITS_OFFSET_TOP, TIMEDIGITS_WIDTH, TIMEDIGITS_HEIGHT, "8", c_t3, f_big },
  {TIMEDIGITS_DIGIT4,   TIMEDIGITS_OFFSET_TOP, TIMEDIGITS_WIDTH, TIMEDIGITS_HEIGHT, "8", c_t3, f_big },
  {0,                   TIMEDIGITS_OFFSET_TOP, TIMEDIGITS_WIDTH, TIMEDIGITS_HEIGHT, "8", c_t3, f_sec },
  {0,                   TIMEDIGITS_OFFSET_TOP, TIMEDIGITS_WIDTH, TIMEDIGITS_HEIGHT, "8", c_t3, f_sec },
  {TIMEDIGITS_SECONDS_DIGIT1,   TIMEDIGITS_SECONDS_OFFSET_TOP,       TIMEDIGITS_SECONDS_WIDTH,       TIMEDIGITS_SECONDS_HEIGHT, "8", c_t4, f_big },
  {TIMEDIGITS_SECONDS_DIGIT2,   TIMEDIGITS_SECONDS_OFFSET_TOP,       TIMEDIGITS_SECONDS_WIDTH,       TIMEDIGITS_SECONDS_HEIGHT, "8", c_t4, f_big },
  {TIMEDIGITS_SECONDS_SEPARATOR,TIMEDIGITS_SECONDS_OFFSET_TOP,       TIMEDIGITS_SECONDS_WIDTH,       TIMEDIGITS_SECONDS_HEIGHT, ":", c_t4, f_big },
  {TIMEDIGITS_SECONDS_DIGIT3,   TIMEDIGITS_SECONDS_OFFSET_TOP,       TIMEDIGITS_SECONDS_WIDTH,       TIMEDIGITS_SECONDS_HEIGHT, "8", c_t4, f_big },
  {TIMEDIGITS_SECONDS_DIGIT4,   TIMEDIGITS_SECONDS_OFFSET_TOP,       TIMEDIGITS_SECONDS_WIDTH,       TIMEDIGITS_SECONDS_HEIGHT, "8", c_t4, f_big },
  {TIMEDIGITS_SECONDS_DIGIT5,   TIMEDIGITS_SECONDS_SMALL_OFFSET_TOP, TIMEDIGITS_SECONDS_SMALL_WIDTH, TIMEDIGITS_SECONDS_SMALL_HEIGHT, "8", c_t4, f_sec },
  {TIMEDIGITS_SECONDS_DIGIT6,   TIMEDIGITS_SECONDS_SMALL_OFFSET_TOP, TIMEDIGITS_SECONDS_SMALL_WIDTH, TIMEDIGITS_SECONDS_SMALL_HEIGHT, "8", c_t4, f_sec },
};

static uint8_t active_digits; //number of active digits (5 regular, 7 including seconds)
static uint8_t start=0; // where to read from digit list. from 7 to 13 is position for time including seconds

bool _true = true;
bool _false = false;

void anim_set_hidden(int16_t layer_num){
    layer_set_hidden(text_layer_get_layer(t_layer[layer_num]),true);
}
int16_t anim_get_hidden(int16_t layer_num){
    return (int16_t) layer_get_hidden(text_layer_get_layer(t_layer[layer_num]));
}
static const PropertyAnimationImplementation layer_hidden_implementation = {
  .base = {
    .update = (AnimationUpdateImplementation) property_animation_update_int16,
  },
  .accessors = {
    .setter = { .int16 = (Int16Setter) anim_set_hidden, },
    .getter = { .int16 = (Int16Getter) anim_get_hidden, },
  },
};

/*
t1  Date
t2  AM/PM/24H Indicator
t3  Clock Shadow
t4  Clock
*/

void load_digits();
void unload_digits();

void timedigits_settings_callback() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "timedigits_settings_callback()");

  unload_digits();
  load_digits();
  timedigits_background_color(color_helper(colors[c_t3], global_settings.Invert));
  timedigits_foreground_color(color_helper(colors[c_t4], global_settings.Invert));
  text_layer_set_text_color(t_layer[t_date], color_helper(colors[c_t1], global_settings.Invert));
  text_layer_set_text_color(t_layer[t_ampm], color_helper(colors[c_t2], global_settings.Invert));

  tick_timer_service_unsubscribe();

  //Somebody set us up the CLOCK
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  //Tick, Tick, Tick, BOOM!
  handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);

  if((global_settings.Blink || global_settings.Seconds) && !powerSaveEngaged) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  }
  else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
    layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), false);
    layer_mark_dirty(text_layer_get_layer(t_layer[t_sep]));
  }
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  if ((units_changed & SECOND_UNIT) && !powerSaveEngaged) {

    if((global_settings.Blink == BLINK_ON) && !powerSaveEngaged) {
      layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), !(tick_time->tm_sec%2));
    }
    else if((global_settings.Blink == BLINK_DOUBLE_RATE) && !powerSaveEngaged) {

      layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), false);

      //needs recreating each time
      separator_animation = property_animation_create(&layer_hidden_implementation, (void *) t_sep, &_false, &_true);
      animation_set_duration((Animation *) separator_animation, 1);
      animation_set_delay((Animation *) separator_animation, 500);
      animation_set_curve((Animation *) separator_animation, AnimationCurveLinear);

      animation_schedule((Animation *)separator_animation);

    }
    else if (global_settings.Blink == BLINK_OFF) {
      layer_set_hidden(text_layer_get_layer(t_layer[t_sep]), false);
    }
    if (global_settings.Seconds) {
      static char digit5[] = "0";
      static char digit6[] = "0";
      digit5[0] = '0'+ tick_time->tm_sec /10;
      digit6[0] = '0'+ tick_time->tm_sec %10;
      text_layer_set_text(t_layer[t_dig5], digit5);
      text_layer_set_text(t_layer[t_dig6], digit6);
    }
  }  //SECOND_UNIT

  if(units_changed & MINUTE_UNIT) {

    if(clock_is_24h_style()) {
      strftime(time_text, sizeof(time_text), "%H%M", tick_time);
    }
    else {
      strftime(time_text, sizeof(time_text), "%I%M", tick_time);
      layer_set_hidden(text_layer_get_layer(t_layer[t_dig1]), (time_text[0] == '0'));
      //layer_set_hidden(text_layer_get_layer(t_layer[t_shadow_dig1]), (time_text[0] == '0'));
    }
    static char digit3[] = "0";
    static char digit4[] = "0";
    digit3[0] = time_text[2];
    digit4[0] = time_text[3];
    text_layer_set_text(t_layer[t_dig3], digit3);
    text_layer_set_text(t_layer[t_dig4], digit4);

    timed_colorset(tick_time->tm_hour,tick_time->tm_min);

    if (global_settings.PowerSave==1) {
      bool isPowerSave=(setting_is_power_save(tick_time->tm_hour,tick_time->tm_min));
      if(isPowerSave && !powerSaveEngaged) {
        //we should save power
        powerSaveEngaged = true;
        bluetooth_deinit();
        battery_deinit();
        update_settings();
      }
      else if (!isPowerSave && powerSaveEngaged) {
        //we should stop saving power

        if (global_settings.Blink || global_settings.Seconds) {
        powerSaveEngaged = false;
        }

        bluetooth_init();
        battery_init();
        update_settings();
      }
    }

  } //MINUTE_UNIT

  if(units_changed & HOUR_UNIT) {
    if(appStarted && global_settings.HourlyVibe && !powerSaveEngaged) {
      //vibe!
      vibes_short_pulse();
    }
    //ToDo: check night settings for color set 2 or regular set
    // use settings_load_colorSet2 for night set
    //
    static char digit1[] = "0";
    static char digit2[] = "0";
    digit1[0] = time_text[0];
    digit2[0] = time_text[1];
    text_layer_set_text(t_layer[t_dig1], digit1);
    text_layer_set_text(t_layer[t_dig2], digit2);

    //AM/PM indicator
    if (!clock_is_24h_style()) {
      if (tick_time->tm_hour >= 12) {
        text_layer_set_text(t_layer[t_ampm], "PM");
      }
      else {
        text_layer_set_text(t_layer[t_ampm], "AM");
      }
    }
  } //HOUR_UNIT



  if (units_changed & DAY_UNIT) {

    static char date_day[4];
    static char date_monthday[3];
    static char date_month[6];
    static char full_date_text[20];

    strftime(date_day, sizeof(date_day), "%a", tick_time);

    strftime(date_monthday,
      sizeof(date_monthday),
      "%d",
      tick_time);

      strftime(date_month,
             sizeof(date_month),
             "%b",
             tick_time);

    /* snprintf(full_date_text,
      sizeof(full_date_text),
      "%s %s %s",
      upcase(date_day),
      date_monthday,
      upcase(date_month)); */

    snprintf(full_date_text,
      sizeof(full_date_text),
      "%s %s",
      upcase(date_day),
      date_monthday);

    text_layer_set_text(t_layer[t_date], full_date_text);

  }

}

void timedigits_foreground_color(GColor foreground) {
  for (int i=0; i<active_digits; i++) {
    text_layer_set_text_color(t_layer[t_dig1+i], foreground);
  }
}

void timedigits_background_color(GColor background) {
  for (int i=0; i<active_digits; i++) {
    text_layer_set_text_color(t_layer[t_shadow_dig1+i], background);
  }
}

void timedigits_color(GColor foreground, GColor background) {
  timedigits_foreground_color(foreground);
  timedigits_background_color(background);
}

TextLayer *timedigits_init_big_digit(int d) {
  TextLayer *text_layer = text_layer_create(GRect(digit_data[d].left, digit_data[d].top, digit_data[d].width, digit_data[d].hight));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, color_helper(colors[digit_data[d].color] , global_settings.Invert));
  text_layer_set_text_alignment(text_layer, GTextAlignmentRight);
  if (digit_data[d].font== f_big) {
    text_layer_set_font(text_layer, font_big);
  }
  else if (digit_data[d].font== f_sec) {
    text_layer_set_font(text_layer, font_small);
  }

  text_layer_set_text(text_layer, digit_data[d].text);
  layer_add_child(clock_layer, text_layer_get_layer(text_layer));
  return text_layer;
}

void load_digits(){
  start = 0;
  active_digits = 5;
  if ((global_settings.Seconds) && !powerSaveEngaged ) {
    start = 7;
    active_digits = 7;
  }
  //Clock Shadow Digits
  for (int i=0; i< active_digits; i++) {
    t_layer[t_shadow_dig1+i] = timedigits_init_big_digit(start + i);
  }
  //Clock Digits
  for (int i=0; i< active_digits; i++) {
    t_layer[t_dig1 + i] = timedigits_init_big_digit(start + i);
  }
}
void unload_digits(){
  for (int i=0; i< active_digits; i++) {
    text_layer_destroy(t_layer[t_shadow_dig1+i]);
    text_layer_destroy(t_layer[t_dig1+i]);
    t_layer[t_shadow_dig1+i] = NULL;
    t_layer[t_dig1+i] = NULL;
  }
}
void timedigits_init() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "timedigits_init()");

  //Center Panel
  center_layer = layer_create(TIMEDIGITS_CENTER);

  layer_add_child(my_window_layer, center_layer);

  //Date - SUN 15
  t_layer[t_date] = text_layer_create_detailed(TIMEDIGITS_DATE, false,
                                GColorClear, color_helper(colors[c_t1], global_settings.Invert),
                                GTextAlignmentRight, font_small);
  layer_add_child(center_layer, text_layer_get_layer(t_layer[t_date]));

  //Indicator - AM/PM/24H
  t_layer[t_ampm] = text_layer_create_detailed(TIMEDIGITS_AMPM, false,
                                GColorClear, color_helper(colors[c_t2], global_settings.Invert),
                                GTextAlignmentLeft, font_tiny);
  if (clock_is_24h_style()) {
    text_layer_set_text(t_layer[t_ampm], "24H");
  }
  layer_add_child(center_layer, text_layer_get_layer(t_layer[t_ampm]));

  //Clock
  clock_layer = layer_create(FULLSCREEN);
  //layer_set_hidden(clock_layer, true);
  layer_add_child(center_layer, clock_layer);

  load_digits();

  //Somebody set us up the CLOCK
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  //Tick, Tick, Tick, BOOM!
  handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
  if((global_settings.Blink || global_settings.Seconds) && !powerSaveEngaged ) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  }
  else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  }

  settings_register_callback(timedigits_settings_callback, SETTINGS_CALLBACK_TIMEDIGITS);
  timedigits_settings_callback();
  //animation_slide_in(text_layer_get_layer(t_layer[t_date]), 600, LEFT);
  //animation_slide_in(text_layer_get_layer(t_layer[t_ampm]), 800, RIGHT);
  //animation_slide_in(clock_layer, 400, LEFT);
}

void timedigits_deinit() {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "timedigits_deinit()");

  tick_timer_service_unsubscribe();

  for (int i=0; i<=tl_last;  i++) {
    if (t_layer[i] != NULL) {
      text_layer_destroy(t_layer[i]);
      t_layer[i] = NULL;
    }
  };

  layer_destroy(clock_layer);
  layer_destroy(center_layer);
}
