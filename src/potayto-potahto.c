#include <pebble.h>

#define MAX_TEMPERATURE_BYTES 5

static Window *window;
static TextLayer *original_value_text_layer;
static TextLayer *converted_value_text_layer;

static char *temperature_string;
static char *converted_temperature_string;

static int temperature = 32;							/* Default 0C */
static int converted_temperature = 0;
static bool english_units = 1;						/* Fahrenheit default */
static bool done = false;

static void update_display(){
	if( english_units ){
		snprintf( temperature_string, MAX_TEMPERATURE_BYTES, "%dF", temperature );
		snprintf( converted_temperature_string, MAX_TEMPERATURE_BYTES, "%dC", converted_temperature );
	}
	else{
		snprintf( temperature_string, MAX_TEMPERATURE_BYTES, "%dC", temperature );
		snprintf( converted_temperature_string, MAX_TEMPERATURE_BYTES, "%dF", converted_temperature );
	}
	
	text_layer_set_text( original_value_text_layer, temperature_string );
  text_layer_set_text( converted_value_text_layer, converted_temperature_string );
  
	layer_mark_dirty( text_layer_get_layer( original_value_text_layer ) );
	layer_mark_dirty( text_layer_get_layer( converted_value_text_layer ) );
}

static int convert_to_celsius( int fahrenheit_temperature ){
	return( ( 5*( fahrenheit_temperature-32 ) )/9 );
}

static int convert_to_fahrenheit( int celsius_temperature ){
	return( ( 9*celsius_temperature )/5 + 32 );
}

static void increase_temperature(){
	temperature += 1;
	
	if( english_units ){
		converted_temperature = convert_to_celsius( temperature );
	}
	else{
		converted_temperature = convert_to_fahrenheit( temperature );
	}
}

static void decrease_temperature(){
	temperature -= 1;
	
	if( english_units ){
		converted_temperature = convert_to_celsius( temperature );
	}
	else{
		converted_temperature = convert_to_fahrenheit( temperature );
	}
}

static void switch_units(){
	int temp = temperature;
	temperature = converted_temperature;
	converted_temperature = temp;
	
	english_units = !english_units;
}

static void select_click_handler( ClickRecognizerRef recognizer, void *context ) {
  switch_units();
	update_display();
}

static void up_click_handler( ClickRecognizerRef recognizer, void *context ) {
  increase_temperature();
	update_display();
}

static void down_click_handler( ClickRecognizerRef recognizer, void *context ) {
  decrease_temperature();
	update_display();
}

static void click_config_provider( void *context ) {
  window_single_click_subscribe( BUTTON_ID_SELECT, select_click_handler );
  window_single_repeating_click_subscribe( BUTTON_ID_UP, 400, up_click_handler );
	window_single_repeating_click_subscribe( BUTTON_ID_DOWN, 400, down_click_handler );
}

static void window_load( Window *window ) {
  Layer *window_layer = window_get_root_layer( window );
  GRect bounds = layer_get_bounds( window_layer );

  original_value_text_layer = text_layer_create( ( GRect ) { .origin = { 0, 10 }, .size = { bounds.size.w, 44 } } );
  text_layer_set_font( original_value_text_layer, fonts_get_system_font( FONT_KEY_BITHAM_42_BOLD ) );
	text_layer_set_text_alignment( original_value_text_layer, GTextAlignmentCenter );
  layer_add_child( window_layer, text_layer_get_layer( original_value_text_layer ) );
	
	converted_value_text_layer = text_layer_create( ( GRect ) { .origin = { 0, ( bounds.size.h/2 )+10 }, .size = { bounds.size.w, 44 } } );
	text_layer_set_font( converted_value_text_layer, fonts_get_system_font( FONT_KEY_BITHAM_42_BOLD ) );
	text_layer_set_text_alignment( converted_value_text_layer, GTextAlignmentCenter );
	layer_add_child( window_layer, text_layer_get_layer( converted_value_text_layer ) );
	
	update_display();
}

static void window_unload( Window *window ) {
  text_layer_destroy( original_value_text_layer );
	text_layer_destroy( converted_value_text_layer );
}

static void init( void ) {
	temperature_string = malloc( MAX_TEMPERATURE_BYTES );
	converted_temperature_string = malloc( MAX_TEMPERATURE_BYTES );
	
	window = window_create();
  window_set_click_config_provider( window, click_config_provider );
  window_set_window_handlers( window, ( WindowHandlers ) {
    .load = window_load,
    .unload = window_unload,
  } );
  window_stack_push( window, true );
}

static void deinit( void ) {
	free( temperature_string );
	free( converted_temperature_string );
  window_destroy( window );
	window_stack_pop_all( false );
}

int main( void ) {
  init();

  APP_LOG( APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window );

  app_event_loop();
  deinit();
}
