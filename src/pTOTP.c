// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pebble.h"

#include "generate.h"
#include "unixtime.h"
#include "timezone.h"

#define TIME_ZONE_KEY   1
#define IS_DST_KEY      2
#define KEY_INDEX_KEY   3

Window *window;

TextLayer *currentKey, *currentCode, *currentTime, *currentOffset;

Layer *barLayer;

unsigned short keyCount = 8;
unsigned short keyIndex = 0;

unsigned short timeZoneIndex = 0;

bool isDST = false;

static char codeDisplayBuffer[7] = "000000";
void redraw(unsigned int code) {  
  for(int x=0;x<6;x++) {
    codeDisplayBuffer[5-x] = '0'+(code % 10);
    code /= 10;
  }

  text_layer_set_text(currentCode, codeDisplayBuffer);
}


bool freshCode = true;

void recode(char *secretKey, bool keyChange) {
  static unsigned int lastCode = 0;
  static unsigned short oldTimeZoneIndex = 255;
  static unsigned int lastQuantizedTimeGenerated = 0;
  static int offset;
  static char offsetText[] = "-12:00";

  const char *key = secretKey;

  if(timeZoneIndex != oldTimeZoneIndex) {
    text_layer_set_text(currentTime, tz_names[timeZoneIndex]);
    oldTimeZoneIndex = timeZoneIndex;
    offset = (tz_offsets[timeZoneIndex]+(isDST ? 3600 : 0));
    snprintf(offsetText, sizeof(offsetText), "%d:%.2d", offset/(60*60), abs((offset/60)%60));
    text_layer_set_text(currentOffset, offsetText);
  }

  unsigned long utcTime = time(NULL)-offset;

  unsigned int quantized_time = utcTime/30;

  //Assuming generating a code is expensive, only generate it if
  //the time has changed or the key has changed.
  if(quantized_time == lastQuantizedTimeGenerated && !keyChange)
    return;

  lastQuantizedTimeGenerated = quantized_time;

  unsigned int code = generateCode(key, quantized_time);

  if(lastCode != code) {
    redraw(code);
    freshCode = true;
  }
}

//Technically not necessary but space-padding is much nicer.
void strip(char *s, int len) {
  while(len >= 0) {
    if(s[len] == ' ' || s[len] == '\0')
      s[len] = '\0';
    else
      break;
    --len;
  }
}

void reload() {
  static unsigned short oldKeyIndex = 255;
  static char secretKey[33];
  static char secretName[33];


  //TODO: Eventually refactor to not require fixed-width data.
  if(keyIndex != oldKeyIndex) {
    resource_load_byte_range(resource_get_handle(RESOURCE_ID_SECRET_KEY), 33*keyIndex, (uint8_t *)&secretKey, 32);
    strip(secretKey, 32);
    resource_load_byte_range(resource_get_handle(RESOURCE_ID_SECRET_NAME), 33*keyIndex, (uint8_t *)&secretName, 32);
    strip(secretName, 32);
    oldKeyIndex = keyIndex;

    text_layer_set_text(currentKey, secretName);
    recode(secretKey, true);
  }
  else
    recode(secretKey, false);
}


// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, void *window) {
  (void)recognizer;
  (void)window;

  ++keyIndex;
  if(keyIndex >= keyCount)
    keyIndex = 0;

  reload();
}


void down_single_click_handler(ClickRecognizerRef recognizer, void *window) {
  (void)recognizer;
  (void)window;

  if(keyIndex == 0)
    keyIndex = keyCount;
  --keyIndex;

  reload();
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *window) {
  (void)recognizer;
  (void)window;

  ++timeZoneIndex;
  if(timeZoneIndex >= TIMEZONE_COUNT)
    timeZoneIndex = 0;

  reload();

  //TODO
}

// This usually won't need to be modified

void click_config_provider(void * context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);

  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up_single_click_handler);

  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down_single_click_handler);
}

void bar_layer_update(Layer *l, GContext* ctx) {
  //static int offset, tick;
  //GSize sz;
  if(freshCode) {
    /*
    sz = graphics_text_layout_get_max_used_size(ctx, codeDisplayBuffer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS),
                                                   GRect(0,32,144,36), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft,
                                                   NULL);

    offset = (144-30*(sz.w/30))/2;
    tick = sz.w/30;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "W: %d, H:%d, O: %d, T: %d", sz.w, sz.h, offset, tick);
    */
    //TODO: graphics_text_layout_get_max_used_size is generating complete garbage.
    freshCode = false;
  }
  graphics_context_set_fill_color(ctx, GColorBlack);
  unsigned short slice = 30-(time(NULL)%30);
  graphics_fill_rect(ctx, GRect(12,0,slice*4,5), 0, GCornerNone);
}


// Standard app init

void handle_init() {
  keyIndex = (persist_exists(KEY_INDEX_KEY) ? persist_read_int(KEY_INDEX_KEY) : 0);

  unsigned char offset = '0';
  unsigned char rawDST = 'N';
  unsigned char rawCount = '1';

  resource_load_byte_range(resource_get_handle(RESOURCE_ID_GMT_OFFSET), 0, &offset, 1);

  timeZoneIndex = (persist_exists(TIME_ZONE_KEY) ? persist_read_int(TIME_ZONE_KEY) : (offset - '0'));

  resource_load_byte_range(resource_get_handle(RESOURCE_ID_IS_DST), 0, &rawDST, 1);

  isDST = (persist_exists(IS_DST_KEY) ? persist_read_bool(IS_DST_KEY) : (rawDST == 'Y'));

  resource_load_byte_range(resource_get_handle(RESOURCE_ID_SECRET_COUNT), 0, &rawCount, 1);

  keyCount = rawCount - '0';
  
  window = window_create();
  window_stack_push(window, true /* Animated */);

  //Great for debugging the layout.
  //window_set_background_color(&window, GColorBlack);

  Layer* rootLayer = window_get_root_layer(window);
  GRect rootLayerRect = layer_get_bounds(rootLayer);
  barLayer = layer_create(GRect(0,70,rootLayerRect.size.w,5));
  layer_set_update_proc(barLayer, bar_layer_update);
  layer_add_child(rootLayer, barLayer);

  currentKey = text_layer_create(GRect(0,0,rootLayerRect.size.w,22));
  text_layer_set_font(currentKey, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(currentKey, GTextAlignmentCenter);
  layer_add_child(rootLayer, text_layer_get_layer(currentKey));

  currentCode = text_layer_create(GRect(0,32,rootLayerRect.size.w,36));
  text_layer_set_font(currentCode, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(currentCode, GTextAlignmentCenter);
  layer_add_child(rootLayer, text_layer_get_layer(currentCode));

  currentTime = text_layer_create(GRect(0,rootLayerRect.size.h-(15+22),(rootLayerRect.size.w/3)*2,22));
  text_layer_set_font(currentTime, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(currentTime, GTextAlignmentLeft);
  layer_add_child(rootLayer, text_layer_get_layer(currentTime));

  currentOffset = text_layer_create(GRect((rootLayerRect.size.w/3)*2,rootLayerRect.size.h-(15+22),rootLayerRect.size.w/3,22));
  text_layer_set_font(currentOffset, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(currentOffset, GTextAlignmentRight);
  layer_add_child(rootLayer, text_layer_get_layer(currentOffset));

  // Attach our desired button functionality
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  reload();
}

void handle_tick(struct tm* tick_time, TimeUnits units_changed) {
  //(void)ctx;
  //(void)event;

  reload();
  layer_mark_dirty(barLayer);
}

void handle_deinit() {
  persist_write_int(TIME_ZONE_KEY, timeZoneIndex);
  persist_write_bool(IS_DST_KEY, isDST);
  persist_write_bool(KEY_INDEX_KEY, keyIndex);
  
  text_layer_destroy(currentOffset);
  text_layer_destroy(currentTime);
  text_layer_destroy(currentCode);
  text_layer_destroy(currentKey);
  layer_destroy(barLayer);
  window_destroy(window);
}

int main() {
  handle_init();
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  app_event_loop();
  handle_deinit();
}
