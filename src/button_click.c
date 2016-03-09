#include <pebble.h>

static Window *window;
static TextLayer *text_layer_aperture;
static TextLayer *text_layer_hyper;
static TextLayer *text_layer_focus;
bool aperture_plus = 1;
bool focus_plus = 1;
char str_aperture[20];
char str_focus[20];
char str_hyper[20];
char str_mm[4];

int mm = 50;

void refreshText() {
  strcpy(str_aperture, "");
  strcat(str_aperture, "f(");
  if (aperture_plus) {
    strcat(str_aperture, "+): ");
  } else {
    strcat(str_aperture, "-): ");
  }
  text_layer_set_text(text_layer_aperture, str_aperture);
  
  strcpy(str_focus, "");
  strcat(str_focus, "mm(");
  if (focus_plus) {
    strcat(str_focus, "+): ");
  } else {
    strcat(str_focus, "-): ");
  }
  snprintf(str_mm, sizeof(str_mm), "%3d", mm);
  strcat(str_focus, str_mm);
  text_layer_set_text(text_layer_focus, str_focus);
  
  text_layer_set_text(text_layer_hyper, "hyper");
}


void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  refreshText();
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  refreshText();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (focus_plus) {
    if (mm < 30) {
      mm = mm + 1;
    } else {
      mm = mm + 5;
    }  
  } else {
    if (mm <= 30) {
      mm = mm - 1;
    } else {
      mm = mm - 5;
    }    
  }
  refreshText();
}

void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  aperture_plus = !aperture_plus;
  refreshText();
}

void up_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  // do nothing
}

void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  focus_plus = !focus_plus;
  refreshText();
}

static void down_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  // no nothing
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 700, up_long_click_handler, up_long_click_release_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 700, down_long_click_handler, down_long_click_release_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer_aperture = text_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(text_layer_aperture, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_aperture));
  
  text_layer_hyper = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(text_layer_hyper, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_hyper));
  
  text_layer_focus = text_layer_create((GRect) { .origin = { 0, 132 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(text_layer_focus, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_focus));
  refreshText();
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer_aperture);
  text_layer_destroy(text_layer_hyper);
  text_layer_destroy(text_layer_focus);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}