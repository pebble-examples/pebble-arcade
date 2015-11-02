#include <pebble.h>
#include "entry.h"

#define NUM_ENTRY_CHARS 3

#define REPEAT_INTERVAL_MS 100

static Window *s_entry_window;
static TextLayer *s_title_text, *s_chars_layers[NUM_ENTRY_CHARS];
static TextLayer *s_selection_layer;
static char* s_highscore_name; // Pointer to put the name into.
static char s_entry_chars[NUM_ENTRY_CHARS][2];
static uint8_t s_selection_index;


static void up_click_handler(ClickRecognizerRef recognizer, void* context) {
  if (s_selection_index < NUM_ENTRY_CHARS) {
    if (s_entry_chars[s_selection_index][0] == 'Z') {
      s_entry_chars[s_selection_index][0] = 'A';
    } else {
      s_entry_chars[s_selection_index][0]++;
    }
    layer_mark_dirty(text_layer_get_layer(s_chars_layers[s_selection_index]));
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void* context) {
  if (s_selection_index < NUM_ENTRY_CHARS) {
    if (s_entry_chars[s_selection_index][0] == 'A') {
      s_entry_chars[s_selection_index][0] = 'Z';
    } else {
      s_entry_chars[s_selection_index][0]--;
    }
    layer_mark_dirty(text_layer_get_layer(s_chars_layers[s_selection_index]));
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
  text_layer_set_text_color(s_chars_layers[s_selection_index], GColorBlack);

  if (s_selection_index == 2) {
    s_selection_index = 0;
  } else {
    ++s_selection_index;
  }

  text_layer_set_text_color(s_chars_layers[s_selection_index], GColorWhite);
  layer_set_frame(text_layer_get_layer(s_selection_layer), GRect(PBL_IF_RECT_ELSE(41, 60) + (20 * s_selection_index), 66, 15, 31));
}

static void click_config_provider(void* context) {
  window_set_click_context(BUTTON_ID_UP, context);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, up_click_handler);

  window_set_click_context(BUTTON_ID_SELECT, context);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);

  window_set_click_context(BUTTON_ID_DOWN, context);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_title_text = text_layer_create(GRect(0, 0, bounds.size.w, 64));
  text_layer_set_text(s_title_text, "NEW HIGH SCORE!");
  text_layer_set_text_alignment(s_title_text, GTextAlignmentCenter);
  text_layer_set_font(s_title_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_title_text));
#ifdef PBL_ROUND
  text_layer_enable_screen_text_flow_and_paging(s_title_text, 8);
#endif

  s_selection_layer = text_layer_create(GRect(PBL_IF_RECT_ELSE(41, 60), 66, 15, 31));
  text_layer_set_background_color(s_selection_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(s_selection_layer));

  for (int i = 0; i < NUM_ENTRY_CHARS; ++i) {
    strncpy(s_entry_chars[i], "A", 2);

    s_chars_layers[i] = text_layer_create(GRect(PBL_IF_RECT_ELSE(41, 60) + (20 * i), 66, 15, 31));
    text_layer_set_font(s_chars_layers[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(s_chars_layers[i], GTextAlignmentCenter);
    text_layer_set_text(s_chars_layers[i], s_entry_chars[i]);
    text_layer_set_background_color(s_chars_layers[i], GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_chars_layers[i]));
  }

  s_selection_index = 0;
  text_layer_set_text_color(s_chars_layers[s_selection_index], GColorWhite);
}

static void window_unload(Window *window) {
  text_layer_destroy(s_title_text);

  for (int i = 0; i < NUM_ENTRY_CHARS; i++) {
    s_highscore_name[i] = s_entry_chars[i][0];
    text_layer_destroy(s_chars_layers[i]);
  }
  text_layer_destroy(s_selection_layer);

  // NULL-terminate the string and return
  s_highscore_name[3] = '\0';
}

void entry_init() {
  s_entry_window = window_create();

  window_set_click_config_provider_with_context(s_entry_window, click_config_provider, (void*)s_entry_window);
  window_set_window_handlers(s_entry_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void entry_deinit() {
  window_destroy(s_entry_window);
}

void entry_get_name(char *name) {
  s_highscore_name = name;
  window_stack_push(s_entry_window, true);
}
