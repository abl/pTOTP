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

#define MY_UUID { 0xF3, 0x61, 0x70, 0x30, 0x87, 0x06, 0x43, 0xB6, 0xAD, 0xDE, 0xD7, 0x3F, 0xDB, 0x38, 0x02, 0x44 }
PBL_APP_INFO(MY_UUID,
             "OTP", "Public Domain",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

TextLayer textLayer;

int oldCode = -1;
unsigned int genTime;
unsigned int lastTick;

//                                 1           2
//                      01234556789012234567899012233
char displayBuffer[] = "TOTPb\n000000\nGMT-12\n30\n\0";
int tz_offset;
static char secretKey[33];

void redraw() {
  int code = oldCode;

  if(code == -1) {
    displayBuffer[6] = ' ';
    displayBuffer[7] = ' ';
    displayBuffer[8] = ' ';
    displayBuffer[9] = ' ';
    displayBuffer[10] = ' ';
    displayBuffer[11] = ' ';

    //displayBuffer[20] = ' ';
    //displayBuffer[21] = ' ';
  }
  else {
    for(int x=0;x<6;x++) {
      displayBuffer[11-x] = '0'+(code % 10);
      code /= 10;
    }

    displayBuffer[20] = '0' + lastTick / 10;
    displayBuffer[21] = '0' + lastTick % 10;
  }

  int tzo = tz_offset;
  if(tzo < 0) {
    tzo = -tzo;
    displayBuffer[16] = '-';
  } else {
    displayBuffer[16] = '+';
  }

  if(tzo > 9) {
    displayBuffer[17] = '1';
    tzo -= 10;
  } else {
    displayBuffer[17] = '0';
  }

  displayBuffer[18] = '0' + tzo;

  text_layer_set_text(&textLayer, displayBuffer);
}

bool recode() {
  const char *key = secretKey;

  genTime = get_unix_time(0);
  lastTick = 30 - genTime % 30;
  genTime /= 30;

  unsigned int cTime = get_unix_time(-tz_offset);

  unsigned int quantized_time = cTime/30;


  int code = generateCode(key, quantized_time);

  if(oldCode != code) {
    oldCode = code;
    return true;
  }
  return false;
}

// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(tz_offset < 12) {
    tz_offset++;
    recode();
    redraw();
  }

}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(tz_offset > -12) {
    tz_offset--;
    recode();
    redraw();
  }
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(recode())
    redraw();
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


// Standard app initialisation

void handle_init(AppContextRef ctx) {
  (void)ctx;
  resource_init_current_app(&APP_RESOURCES);

  uint8_t offset;

  resource_load_byte_range(resource_get_handle(RESOURCE_ID_GMT_OFFSET), 0, &offset, 1);
  tz_offset = offset;
  tz_offset -= '<';
  resource_load_byte_range(resource_get_handle(RESOURCE_ID_SECRET_KEY), 0, (uint8_t *)&secretKey, 32);

  for(uint8_t x=0; x < 32; x++) {
    if(secretKey[x] == '\n') {
      secretKey[x] = 0;
      break;
    }
  }
  secretKey[32] = 0;

  window_init(&window, "Button App");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&textLayer, window.layer.frame);
  text_layer_set_text(&textLayer, displayBuffer);
  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  layer_add_child(&window.layer, &textLayer.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *event) {
  (void)ctx;
  (void)event;

  PblTm curr_time;
  get_time(&curr_time);

  unsigned int curTime = curr_time.tm_sec;
  unsigned int nextTick = 30 - curTime % 30;
  if(nextTick > lastTick) {
    recode();
  }
  lastTick = nextTick;

  redraw();
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
