#include <pebble.h>
   
#define KEY_TEMPERATURE 1
#define KEY_CONDITIONS 2
#define KEY_SCALE 3
#define KEY_TEST 4

static Window *window;

static TextLayer *text_time_layer;
static TextLayer *minute_layer;
static GFont time_font;

TextLayer *text_date_layer;
static GFont *date_font;


static TextLayer *s_weather_layer;
static GFont s_weather_font;


static BitmapLayer *background_layer;
static GBitmap *background_image;

static bool initiate_watchface = true;

//Random Number Setup
int NUM_MUG_SHOTS = 11;
int seed_mugs2;
int start_number_mug, random_mug;

const int MUG_SHOTS_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_LING,
  RESOURCE_ID_CAMPBELL,
  RESOURCE_ID_CHRIS,
  RESOURCE_ID_WEASEL,
  RESOURCE_ID_MCBRIDE,
  RESOURCE_ID_GENERAL,
  RESOURCE_ID_VIPER,
  RESOURCE_ID_HAWK,
  RESOURCE_ID_OWL,
  RESOURCE_ID_PYRO,
  RESOURCE_ID_JIMMY
};

const int CODEC_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_Codec2_WHITE,
  RESOURCE_ID_Codec4_WHITE,
  RESOURCE_ID_Codec6_WHITE,
  RESOURCE_ID_Codec8_WHITE
};



const int BT_WHITE_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_DEEPTHROAT_BLUETOOTH_WHITE};

const int BT_BLACK_IMAGE_RESOURCE_IDS[] = {
 	RESOURCE_ID_DEEPTHROAT_BLUETOOTH_BLACK};

#define TOTAL_mug_amount 2
static GBitmap *mug_amount_images[TOTAL_mug_amount];
static BitmapLayer *mug_amount_layers[TOTAL_mug_amount];

#define TOTAL_BT_DIGITS 2
static GBitmap *bt_digits_images[TOTAL_BT_DIGITS];
static BitmapLayer *bt_digits_layers[TOTAL_BT_DIGITS];

#define TOTAL_CODEC 4
static GBitmap *codec_images[TOTAL_CODEC];
static BitmapLayer *codec_layers[TOTAL_CODEC];


//Bitmap Container
static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
GBitmap *old_image = *bmp_image;

 	*bmp_image = gbitmap_create_with_resource(resource_id);
 	GRect frame = (GRect) {
   	.origin = origin,
   	.size = (*bmp_image)->bounds.size
    //.size = gbitmap_get_bounds(*bmp_image).size
      
};
 	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
 	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

 	if (old_image != NULL) {
 	gbitmap_destroy(old_image);
}
}


// Bluetooth Connectivity
static void handle_bluetooth(bool connected) {
if (connected) {
// Normal Mode 
if (initiate_watchface) {
   	layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), true);
   	layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), true);
}
// On disconnection vibrate twice
else {
    layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), true);
   	layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), true);
vibes_double_pulse();
}
}
else {
  
// If started in disconnection display Deepthroat, no vibration
if (initiate_watchface) {
  layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), false);
bitmap_layer_set_compositing_mode(bt_digits_layers[0], GCompOpOr);
set_container_image(&bt_digits_images[0], bt_digits_layers[0], BT_WHITE_IMAGE_RESOURCE_IDS[0], GPoint(4, 40));
  layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), false);
    bitmap_layer_set_compositing_mode(bt_digits_layers[1], GCompOpClear);
set_container_image(&bt_digits_images[1], bt_digits_layers[1], BT_BLACK_IMAGE_RESOURCE_IDS[0], GPoint(4, 40));
}
  
// On disconnection display Deepthroat and vibrate three times
else {
  layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), false);
bitmap_layer_set_compositing_mode(bt_digits_layers[0], GCompOpOr);
set_container_image(&bt_digits_images[0], bt_digits_layers[0], BT_WHITE_IMAGE_RESOURCE_IDS[0], GPoint(4, 40));
  layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[1]), false);
    bitmap_layer_set_compositing_mode(bt_digits_layers[1], GCompOpClear);
set_container_image(&bt_digits_images[1], bt_digits_layers[1], BT_BLACK_IMAGE_RESOURCE_IDS[0], GPoint(4, 40));

vibes_enqueue_custom_pattern( (VibePattern) {
   	.durations = (uint32_t []) {100, 100, 100, 100, 100},
   	.num_segments = 5
} );
}}}

  static void update_display(struct tm *current_time) {
 	if( ((current_time->tm_min == 0) && (current_time->tm_sec == 0)) || (initiate_watchface == true) ){ 

  if (initiate_watchface){
  start_number_mug = (current_time->tm_sec) + (current_time->tm_min) + NUM_MUG_SHOTS;
}
    
  static long seed_mugs = 100;
  seed_mugs  = (((seed_mugs * 214013L + 2531011L) >> 16) & 32767);
  seed_mugs2 = seed_mugs + start_number_mug;
  random_mug = (seed_mugs2 % NUM_MUG_SHOTS);

set_container_image(&mug_amount_images[1], mug_amount_layers[1], MUG_SHOTS_IMAGE_RESOURCE_IDS[random_mug], GPoint(4, 40));
}
}



//--BATTERY
static uint8_t battery_level;
//static bool battery_plugged;
static Layer *battery_layer;

//--BATTERY LAYER
void battery_layer_update_callback(Layer *layer, GContext *ctx) {

  	if (battery_level > 75) {
    set_container_image(&codec_images[0], codec_layers[0], CODEC_IMAGE_RESOURCE_IDS[3], GPoint(45, 49));
    bitmap_layer_set_compositing_mode(codec_layers[0], GCompOpOr);

      } 
      else if (battery_level > 50 && battery_level <= 75){
    set_container_image(&codec_images[0], codec_layers[0], CODEC_IMAGE_RESOURCE_IDS[2], GPoint(45, 49));
    bitmap_layer_set_compositing_mode(codec_layers[0], GCompOpOr);
      
    }
      else if (battery_level > 25 && battery_level <= 50){
    set_container_image(&codec_images[0], codec_layers[0], CODEC_IMAGE_RESOURCE_IDS[1], GPoint(45, 49));
    bitmap_layer_set_compositing_mode(codec_layers[0], GCompOpOr);
      
    }
      else if (battery_level <= 25){
    set_container_image(&codec_images[0], codec_layers[0], CODEC_IMAGE_RESOURCE_IDS[0], GPoint(45, 49));
    bitmap_layer_set_compositing_mode(codec_layers[0], GCompOpOr);
      
    }
  	else {	
 	}
  
}

//--HANDLE BATTERY
void battery_state_handler(BatteryChargeState charge) {
	battery_level = charge.charge_percent;
  layer_mark_dirty(battery_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00";
  static char buffer2[] = "00";
  static char date_text[] = "00.00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00"), "%H", tick_time);
    strftime(buffer2, sizeof("00"), "%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00"), "%I", tick_time);
    strftime(buffer2, sizeof("00"), "%M", tick_time);
  }
  
  // Only update the date when it's changed.
  strftime(date_text, sizeof(date_text), "%m.%d", tick_time);
  text_layer_set_text(text_date_layer, date_text);

  // Display this time on the TextLayer
  text_layer_set_text(text_time_layer, buffer);
  text_layer_set_text(minute_layer, buffer2);
  
}

static void main_window_load(Window *window) {
  
  Layer *window_layer = window_get_root_layer(window);
  
  //Create GBitmap, then set to created BitmapLayer
  background_image = gbitmap_create_with_resource(RESOURCE_ID_NEW_BACKGROUND);
  background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(background_layer, background_image);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(background_layer));
  
  // Create Hour TextLayer
  text_time_layer = text_layer_create(GRect(28, 95, 139, 50));
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_text(text_time_layer, "00");
    
  //Create Minute TextLayer
  minute_layer = text_layer_create(GRect(83, 95, 139, 50));
  text_layer_set_background_color(minute_layer, GColorClear);
  text_layer_set_text_color(minute_layer, GColorWhite);
  text_layer_set_text(minute_layer, "00");
  
  //Create GFont
  time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_40));
  date_font  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DIGITAL_17));

  //Apply to TextLayer
  text_layer_set_font(text_time_layer, time_font);
  text_layer_set_font(minute_layer, time_font);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minute_layer));

  
  // Create temperature Layer
  s_weather_layer = text_layer_create(GRect(0, 138, 144, 25));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Sneaking...");

  
  // Create second custom font, apply it and add to Window
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Weather_20));
  text_layer_set_font(s_weather_layer, s_weather_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  
  
  //BATTERY
 	BatteryChargeState initial = battery_state_service_peek();
 	battery_level = initial.charge_percent;
 	battery_layer = layer_create(GRect(0,0,144,168));
 	layer_set_update_proc(battery_layer, &battery_layer_update_callback);
 	layer_add_child(window_get_root_layer(window), battery_layer);
    
   // Creat DateLayer
  text_date_layer = text_layer_create(GRect(66, 63, 139, 50));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, date_font);
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_weather_font);

  //Destroy GBitmap
  gbitmap_destroy(background_image);

  //Destroy BitmapLayer
  bitmap_layer_destroy(background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(text_time_layer);
  text_layer_destroy(minute_layer);
  text_layer_destroy(text_date_layer);
  text_layer_destroy(s_weather_layer);


 	for (int i = 0; i < TOTAL_mug_amount; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(mug_amount_layers[i]));
   	gbitmap_destroy(mug_amount_images[i]);
   	bitmap_layer_destroy(mug_amount_layers[i]);
}
   	for (int i = 0; i < TOTAL_BT_DIGITS; i++) {
     	layer_remove_from_parent(bitmap_layer_get_layer(bt_digits_layers[i]));
     	gbitmap_destroy(bt_digits_images[i]);
     	bitmap_layer_destroy(bt_digits_layers[i]);
 	}
  
  	for (int i = 0; i < TOTAL_CODEC; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(codec_layers[i]));
   	gbitmap_destroy(codec_images[i]);
   	bitmap_layer_destroy(codec_layers[i]);

  
}

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();

  }

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  int temperature;
  int Kelvin = persist_read_int(KEY_TEMPERATURE);
  int finalTemp = Kelvin;
  int test = persist_read_int(KEY_TEST);
    
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_SCALE:
      if(strcmp(t->value->cstring, "F") == 0){
        persist_write_int(KEY_TEST, 0);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      else if(strcmp(t->value->cstring, "C") == 0){
        persist_write_int(KEY_TEST, 1);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      break;
    case KEY_TEMPERATURE:
      if(test == 0){
      temperature = (int)t->value->int32;
      persist_write_int(KEY_TEMPERATURE, temperature);
      Kelvin = persist_read_int(KEY_TEMPERATURE);
      finalTemp = (Kelvin - 273.15) * 1.8 + 32;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", finalTemp);
      }
      else if(test == 1){
      temperature = (int)t->value->int32;
      persist_write_int(KEY_TEMPERATURE, temperature);
      Kelvin = persist_read_int(KEY_TEMPERATURE);
      finalTemp = Kelvin - 273.15;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", finalTemp);
      }
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

 
static void init() {
  // Create main Window element and assign to pointer
  window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
  Layer *window_layer = window_get_root_layer(window);

// Create mugs layer
 	GRect dummy_frame = { {0, 0}, {0, 0} };
for (int i = 0; i < TOTAL_mug_amount; ++i) {
   	mug_amount_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(mug_amount_layers[i]));
}

//Create Bluetooth layer
 	for (int i = 0; i < TOTAL_BT_DIGITS; ++i) {
   	bt_digits_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(bt_digits_layers[i]));
}
  handle_bluetooth(bluetooth_connection_service_peek());
  
  //Create Bluetooth layer
 	for (int i = 0; i < TOTAL_CODEC; ++i) {
   	codec_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(codec_layers[i]));
}

// Avoids a blank screen on watch start.
 	time_t now = time(NULL);
 	struct tm *tick_time = localtime(&now);
  
 	update_display(tick_time);

 	initiate_watchface = false;
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void deinit() {
  // Destroy Window
  window_destroy(window);
  
  battery_state_service_unsubscribe();
  
  bluetooth_connection_service_unsubscribe();
  
  fonts_unload_custom_font(date_font);
}

int main(void) {
  init();
  battery_state_service_subscribe (&battery_state_handler);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  app_event_loop();
  deinit();
}
