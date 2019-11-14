#include <pebble.h>
#include <ctype.h>
#include "helpers.h"
#include "_globals.h"

void duration_to_time(int duration_s, int *hours, int *minutes) {
  *hours = (duration_s / 3600) ?: 0;
  *minutes = ((duration_s % 3600) / 60) ?: 0;
}

void format_commas(int n, char *out)
{
    int c;
    char buf[20];
    char *p;

    snprintf(buf, sizeof(buf), "%d", n);
    c = 2 - strlen(buf) % 3;
    for (p = buf; *p != 0; p++) {
       *out++ = *p;
       if (c == 1) {
           *out++ = ',';
       }
       c = (c + 1) % 3;
    }
    *--out = 0;
}

const char* get_gcolor_text(GColor m_color){
  if(gcolor_equal(m_color, GColorClear)) {
    return "GColorClear";
  }
  if(gcolor_equal(m_color, GColorWhite)) {
    return "GColorWhite";
  }
  if(gcolor_equal(m_color, GColorBlack)) {
    return "GColorBlack";
  }
  return "";
}

char *upcase(char *str) {
  char *s = str;
  while (*s) {
    *s++ = toupper((int)*s);
  }
  return str;
}

// quick implementation, can be improved
uint8_t hex_to_num (char h){
  uint8_t rslt=0;
  if ((h>='0') & (h<='9'))
    rslt= h-'0';
  else if ((h>='A') & (h<='F'))
    rslt = h-'A'+10;
  return rslt;
}

GColor color_inverted(GColor source) {
    GColor inverted = source;
  if(gcolor_equal(source, GColorBlack))
    inverted= GColorWhite;
  if(gcolor_equal(source, GColorWhite))
    inverted= GColorBlack;
  #ifdef PBL_COLOR
    if(!gcolor_equal(source, GColorClear)) //GColorClear should not change
      inverted.argb= source.argb ^ 0b00111111;
  #endif
  return inverted;
}


/*
static int animation_slot = 0;
static Animation animations[10];

void animation_slide_in_cleanup() {
     for(int i = 0; i < 10; i++) {
        property_animation_destroy(animations[i]);
    }
}
*/

void animation_slide_in(Layer *layer, int delay, direction_t direction) {
  layer_set_hidden(layer, false);
  return;

  /*
  static PropertyAnimation *s_property_animation;
  int startX = 0, startY = 0, endX = 0, endY = 0;

  GRect from_frame = layer_get_frame(layer);
  endX = from_frame.origin.x;
  endY = from_frame.origin.y;

  GRect to_frame = GRect(endX, endY, from_frame.size.w, from_frame.size.h);

  if(direction==LEFT || direction==RIGHT) {
    startX = 144 + from_frame.size.w; //way off screen left
    if(direction == RIGHT) {
      startX = startX*-1; //way off screen right
    }
    from_frame.origin.x=startX;
  }
  else if(direction==UP || direction==DOWN) {
    startY = 168 + from_frame.size.h; //way off screen top
    if(direction == DOWN) {
      startY = startY*-1; //way off screen bottom
    }
    from_frame.origin.y=startY;
  }

  layer_set_frame(layer, from_frame);
  layer_set_hidden(layer, false);

  // Create the animation
  s_property_animation = property_animation_create_layer_frame(layer, &from_frame, &to_frame);
  animation_set_duration((Animation*)s_property_animation, ANIM_DURATION);
  animation_set_delay((Animation*)s_property_animation, delay);
  animation_set_curve((Animation*)s_property_animation, AnimationCurveEaseInOut);

  animation_set_handlers((Animation*) s_property_animation, (AnimationHandlers) {
    .stopped = (AnimationStoppedHandler) animation_slide_in_stopped,
  }, NULL);

  // Schedule to occur ASAP with default settings
  animation_schedule((Animation*) s_property_animation);
  */

  //keep track of started animations
  //animations[animation_slot] = (Animation*) s_property_animation;
  //animation_slot++;

}

TextLayer *text_layer_create_detailed(GRect frame, bool hidden, GColor background_color, GColor foreground_color, GTextAlignment alignment, GFont font) {
  TextLayer *text_layer = text_layer_create(frame);
  layer_set_hidden(text_layer_get_layer(text_layer), hidden);
  text_layer_set_background_color(text_layer, background_color);
  text_layer_set_text_color(text_layer, foreground_color);
  text_layer_set_text_alignment(text_layer, alignment);
  text_layer_set_font(text_layer, font);
  return text_layer;
}

GColor color_helper(GColor color, uint8_t inverted) {
  #ifdef PBL_BW
    if(inverted) {
      if(gcolor_equal(color, GColorBlack)) {
        color = GColorWhite;
      }
      else {
        color = GColorBlack;
      }
    }
  #else
    if(inverted) {
      color = color_inverted(color);
    }
  #endif
  return color;
}
