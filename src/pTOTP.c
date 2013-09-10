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

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "generate.h"
#include "unixtime.h"
#include "timezone.h"

#define MY_UUID { 0xF3, 0x61, 0x70, 0x30, 0x87, 0x06, 0x43, 0xB6, 0xAD, 0xDE, 0xD7, 0x3F, 0xDB, 0x38, 0x02, 0x44 }
PBL_APP_INFO(MY_UUID,
             "pTOTP", "Public Domain",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

TextLayer currentKey, currentCode, currentTime, currentOffset;

Layer barLayer;

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

  text_layer_set_text(&currentCode, codeDisplayBuffer);
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
    text_layer_set_text(&currentTime, tz_names[timeZoneIndex]);
    oldTimeZoneIndex = timeZoneIndex;
    offset = (tz_offsets[timeZoneIndex]+(isDST ? 3600 : 0))/60;
    snprintf(offsetText, sizeof(offsetText), "%d:%.2d", offset/60, abs(offset%60));
    text_layer_set_text(&currentOffset, offsetText);
  }

  unsigned int utcTime = time(NULL)-offset;
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

    text_layer_set_text(&currentKey, secretName);
    recode(secretKey, true);
  }
  else
    recode(secretKey, false);
}


// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  ++keyIndex;
  if(keyIndex >= keyCount)
    keyIndex = 0;

  reload();
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(keyIndex == 0)
    keyIndex = keyCount;
  --keyIndex;

  reload();
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  ++timeZoneIndex;
  if(timeZoneIndex >= TIMEZONE_COUNT)
    timeZoneIndex = 0;

  reload();

  //TODO
}

// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
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

void handle_init(AppContextRef ctx) {
  (void)ctx;
  resource_init_current_app(&APP_RESOURCES);

  keyIndex = 0;

  unsigned char offset = '0';
  unsigned char rawDST = 'N';

  resource_load_byte_range(resource_get_handle(RESOURCE_ID_GMT_OFFSET), 0, &offset, 1);

  timeZoneIndex = offset - '0';

  resource_load_byte_range(resource_get_handle(RESOURCE_ID_IS_DST), 0, &rawDST, 1);

  isDST = (rawDST == 'Y');
  
  window_init(&window, "pTOTP");
  window_stack_push(&window, true /* Animated */);

  //Great for debugging the layout.
  //window_set_background_color(&window, GColorBlack);

  layer_init(&barLayer, GRect(0,70,window.layer.frame.size.w,5));
  barLayer.update_proc = &bar_layer_update;
  layer_add_child(&window.layer, &barLayer);

  text_layer_init(&currentKey, GRect(0,0,window.layer.frame.size.w,22));
  text_layer_set_font(&currentKey, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&currentKey, GTextAlignmentCenter);
  layer_add_child(&window.layer, &currentKey.layer);

  text_layer_init(&currentCode, GRect(0,32,window.layer.frame.size.w,36));
  text_layer_set_font(&currentCode, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(&currentCode, GTextAlignmentCenter);
  layer_add_child(&window.layer, &currentCode.layer);

  text_layer_init(&currentTime, GRect(0,window.layer.frame.size.h-(15+22),(window.layer.frame.size.w/4)*3,22));
  text_layer_set_font(&currentTime, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&currentTime, GTextAlignmentLeft);
  layer_add_child(&window.layer, &currentTime.layer);

  text_layer_init(&currentOffset, GRect((window.layer.frame.size.w/4)*3,window.layer.frame.size.h-(15+22),window.layer.frame.size.w/4,22));
  text_layer_set_font(&currentOffset, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&currentOffset, GTextAlignmentRight);
  layer_add_child(&window.layer, &currentOffset.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);

  reload();
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *event) {
  //(void)ctx;
  //(void)event;

  reload();
  layer_mark_dirty(&barLayer);

}


void pbl_main(void *params) {

  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
