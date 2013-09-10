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

TextLayer currentKey, currentCode, currentTime;

unsigned short keyCount = 8;
unsigned short keyIndex = 0;

unsigned short timeZoneIndex = 0;

void redraw(unsigned int code) {
  static char codeDisplayBuffer[7] = "000000";
  
  for(int x=0;x<6;x++) {
    codeDisplayBuffer[6-x] = '0'+(code % 10);
    code /= 10;
  }

  text_layer_set_text(&currentCode, codeDisplayBuffer);
}


void recode(char *secretKey, bool keyChange) {
  static unsigned int lastCode = 0;
  static unsigned short oldTimeZoneIndex = 255;
  static unsigned int lastQuantizedTimeGenerated = 0;

  const char *key = secretKey;

  if(timeZoneIndex != oldTimeZoneIndex) {
    text_layer_set_text(&currentTime, TIMEZONES[timeZoneIndex].tz_name);
    oldTimeZoneIndex = timeZoneIndex;
  }

  unsigned int utcTime = time(NULL)-TIMEZONES[timeZoneIndex].tz_offset;
  unsigned int quantized_time = utcTime/30;

  //Assuming generating a code is expensive, only generate it if
  //the time has changed or the key has changed.
  if(quantized_time == lastQuantizedTimeGenerated && !keyChange)
    return;

  lastQuantizedTimeGenerated = quantized_time;

  unsigned int code = generateCode(key, quantized_time);

  if(lastCode != code) {
    redraw(code);
  }
}

void reload() {
  static unsigned short oldKeyIndex = 255;
  static char secretKey[33];
  static char secretName[33];


  //TODO: Refactor to not require fixed-width data.
  if(keyIndex != oldKeyIndex) {
    resource_load_byte_range(resource_get_handle(RESOURCE_ID_SECRET_KEY), 33*keyIndex, (uint8_t *)&secretKey, 32);
    resource_load_byte_range(resource_get_handle(RESOURCE_ID_SECRET_NAME), 33*keyIndex, (uint8_t *)&secretName, 32);
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


// Standard app init

void handle_init(AppContextRef ctx) {
  (void)ctx;
  resource_init_current_app(&APP_RESOURCES);

  keyIndex = 0;
  reload();

  unsigned char offset = '0';

  resource_load_byte_range(resource_get_handle(RESOURCE_ID_GMT_OFFSET), 0, &offset, 1);
  timeZoneIndex = offset - '0';

  window_init(&window, "pTOTP");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&currentKey, window.layer.frame);
  text_layer_set_font(&currentKey, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(&window.layer, &currentKey.layer);

  text_layer_init(&currentKey, window.layer.frame);
  text_layer_set_font(&currentKey, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(&window.layer, &currentKey.layer);

  text_layer_init(&currentKey, window.layer.frame);
  text_layer_set_font(&currentKey, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(&window.layer, &currentKey.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *event) {
  (void)ctx;
  (void)event;

  reload();
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
