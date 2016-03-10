#include <pebble.h>

static Window *window;
static TextLayer *text_layer_aperture;
static TextLayer *text_layer_sensor;
static TextLayer *text_layer_hyper;
static TextLayer *text_layer_focus;
bool aperture_plus = 1;
bool focus_plus = 1;
char str_aperture[20];
char str_focus[20];
char str_hyper[20];
char str_mm[4];
char str_fstop[5];
double apertures[] = {
  1.0d,
  1.4d,
  1.8d,
  2.0d,
  2.8d,
  4.0d,
  5.6d,
  8.0d,
  11.0d,
  16.0d,
  22.0d
};
int aperture_elements = 11; 
int aperturepos = 5; // f/4.0 as default
int mm = 50; // default 50 mm focal lenght

int cof = 0; // circle of confusion, set for 35mm full-frame as default
double cofs[] = {
  0.03d, // 35 mm full-frame
  0.023d, // APS-H Canon
  0.018d, // APS-C Canon
  0.019d, // APS-C Nikon, Pentax, Sony
  0.015d, // Four-Thirds
  0.011d // Nikon 1
};
char *str_cofs[] = {
  "35 mm FF",
  "APS-H Canon",
  "APS-C Canon",
  "APS-C Nikon/Pentax/Sony",
  "Four-Thirds",
  "Nikon 1"
};
int cofs_elements = 6;

double calcHyperFocalDistance(double f, int mm, double cof) {
  return (((double)(mm*mm)/(f*cof))+(double)mm)/(double)1000;
}

void refreshText() {
  strcpy(str_aperture, "");
  strcat(str_aperture, "f-stop(");
  if (aperture_plus) {
    strcat(str_aperture, "+): f/");
  } else {
    strcat(str_aperture, "-): f/");
  }
  snprintf(str_fstop, sizeof(str_fstop), "%d.%1d", (int)apertures[aperturepos], (int)(apertures[aperturepos]*10 - (int)apertures[aperturepos]*10));
  strcat(str_aperture, str_fstop);  
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
  
  text_layer_set_text(text_layer_sensor, str_cofs[cof]);
  
  double distance = calcHyperFocalDistance(apertures[aperturepos], mm, cofs[cof]);
  snprintf(str_hyper, sizeof(str_hyper), "%d.%1d m", (int)distance, (int)(distance*10 - (int)distance*10));
  text_layer_set_text(text_layer_hyper, str_hyper);
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (cof == cofs_elements - 1) {
    cof = 0;
  } else {
    cof = cof + 1;
  }
  refreshText();
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (aperture_plus) {
    if (aperturepos == aperture_elements-1) {
      aperturepos = 0;
    } else {
      aperturepos = aperturepos + 1;
    }
  } else {
    if (aperturepos == 0) {
      aperturepos = aperture_elements - 1;
    } else {
      aperturepos = aperturepos - 1;
    }
  }
  refreshText();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (focus_plus) {
    if (mm < 30) {
      mm = mm + 1;
    } else if (mm < 110) {
      mm = mm + 5;
    } else {
      mm = mm + 20;
    }
  } else {
    if (mm <= 30) {
      mm = mm - 1;
    } else if (mm > 110) {
      mm = mm - 20;
    } else {
      mm = mm - 5;
    } 
    if (mm == 0) {
      mm = 1;
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
  window_long_click_subscribe(BUTTON_ID_UP, 500, up_long_click_handler, up_long_click_release_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 500, down_long_click_handler, down_long_click_release_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  
  
  text_layer_aperture = text_layer_create((GRect) { .origin = { 0, 10 }, .size = { bounds.size.w, 30 } });
  text_layer_set_text_alignment(text_layer_aperture, GTextAlignmentCenter);
  text_layer_set_font(text_layer_aperture, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_aperture));
  
  text_layer_sensor = text_layer_create((GRect) { .origin = { 0, 50 }, .size = { bounds.size.w, 15 } });
  text_layer_set_text_alignment(text_layer_sensor, GTextAlignmentCenter);
  text_layer_set_font(text_layer_sensor, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_sensor));
  
  text_layer_hyper = text_layer_create((GRect) { .origin = { 0, 65 }, .size = { bounds.size.w, 30 } });
  text_layer_set_text_alignment(text_layer_hyper, GTextAlignmentCenter);
  text_layer_set_font(text_layer_hyper, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_hyper));
  
  text_layer_focus = text_layer_create((GRect) { .origin = { 0, 120 }, .size = { bounds.size.w, 30 } });
  text_layer_set_text_alignment(text_layer_focus, GTextAlignmentCenter);
  text_layer_set_font(text_layer_focus, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_focus));
  refreshText();
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer_aperture);
  text_layer_destroy(text_layer_sensor);
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