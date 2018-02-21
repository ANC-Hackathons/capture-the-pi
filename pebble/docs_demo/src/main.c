#include <pebble.h>

// Initial Window
static Window *s_main_window;

// Define constants for your service ID, attribute ID 
// and buffer size of your attribute.
static const SmartstrapServiceId s_service_id = 0x1001;
static const SmartstrapAttributeId s_attribute_id = 0x0001;
static const int s_buffer_length = 64;

// Declare an attribute pointer
static SmartstrapAttribute *s_attribute;

static char* smartstrap_result_to_string(SmartstrapResult result) {
  switch(result) {
    case SmartstrapResultOk:
      return "SmartstrapResultOk";
    case SmartstrapResultInvalidArgs:
      return "SmartstrapResultInvalidArgs";
    case SmartstrapResultNotPresent:
      return "SmartstrapResultNotPresent";
    case SmartstrapResultBusy:
      return "SmartstrapResultBusy";
    case SmartstrapResultServiceUnavailable:
      return "SmartstrapResultServiceUnavailable";
    case SmartstrapResultAttributeUnsupported:
      return "SmartstrapResultAttributeUnsupported";
    case SmartstrapResultTimeOut:
      return "SmartstrapResultTimeOut";
    default:
      return "Not a SmartstrapResult value!";
  }
}

static void strap_availability_handler(SmartstrapServiceId service_id,
                                           bool is_available) {
  // A service's availability has changed
  APP_LOG(APP_LOG_LEVEL_INFO, "Service %d is %s available",
                                (int)service_id, is_available ? "now" : "NOT");
}

static void strap_write_handler(SmartstrapAttribute *attribute,
                                    SmartstrapResult result) {
  // A write operation has been attempted
  if(result != SmartstrapResultOk) {
    // Handle the failure
    APP_LOG(APP_LOG_LEVEL_ERROR, "Smartstrap error occured: %s",
                                          smartstrap_result_to_string(result));
  }
}

static void strap_read_handler(SmartstrapAttribute *attribute,
                                   SmartstrapResult result, const uint8_t *data,
                                                                  size_t length) {
  if(result == SmartstrapResultOk) {
    // Data has been read into the data buffer provided
    APP_LOG(APP_LOG_LEVEL_INFO, "Smartstrap sent: %s", (char*)data);
  } else {
    // Some error has occured
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error in read handler: %d", (int)result);
  }
}

static void strap_notify_handler(SmartstrapAttribute *attribute) {
  // The smartstrap has emitted a notification for this attribute
  APP_LOG(APP_LOG_LEVEL_INFO, "Attribute with ID %d sent notification",
                        (int)smartstrap_attribute_get_attribute_id(attribute));

  // Some data is ready, let's read it
  smartstrap_attribute_read(attribute);
}

/*static void read_attribute() {*/
/*  SmartstrapResult result = smartstrap_attribute_read(attribute);*/
/*  if(result != SmartstrapResultOk) {*/
/*    APP_LOG(APP_LOG_LEVEL_ERROR, "Error reading attribute: %s",*/
/*                                        smartstrap_result_to_string(result));*/
/*  }*/
/*}*/

static void main_window_load(Window *window) {

}

static void main_window_unload(Window *window) {

}

static void init() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "STARTING APP");

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
 
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Create the attribute, and allocate a buffer for its data
  s_attribute = smartstrap_attribute_create(s_service_id, s_attribute_id,
                                                          s_buffer_length);

  // Subscribe to the smartstrap events
  smartstrap_subscribe((SmartstrapHandlers) {
    .availability_did_change = strap_availability_handler,
    .did_read = strap_read_handler,
    .did_write = strap_write_handler,
    .notified = strap_notify_handler
  });
}

static void deinit() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ENDING APP");

  // Destroy Window
  window_destroy(s_main_window);

  // Stop getting callbacks
  smartstrap_unsubscribe();

  // Destroy this attribute
  smartstrap_attribute_destroy(s_attribute);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
