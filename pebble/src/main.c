#include <pebble.h>

#define TIMEOUT_MS 1000
#define MAX_READ_SIZE 100

static Window *s_main_window;
static TextLayer *s_status_layer;
static TextLayer *s_attr_text_layer;
static TextLayer *s_raw_text_layer;
static char s_text_buffer1[20];
static char s_text_buffer2[20];
static char s_score_buffer[20];
static SmartstrapAttribute *s_attr_attribute;
static SmartstrapAttribute *s_team_attribute;
static SmartstrapAttribute *s_score_attribute;
static SmartstrapAttribute *s_kill_attribute;

// Possible team values:
//   0 = unset
//   1 = Red
//   2 = Blue
static uint32_t myTeam = 0;
static bool teamChosen = false;
static uint32_t redScore = 0;
static uint32_t blueScore = 0;

// Define constants for your service ID, attribute ID
// and buffer size of your attribute.
static const SmartstrapServiceId s_service_id = 0x1001;
static const SmartstrapAttributeId s_attr_attribute_id = 0x0001;
static const SmartstrapAttributeId s_team_attribute_id = 0x0002;
static const SmartstrapAttributeId s_score_attribute_id = 0x0003;
static const SmartstrapAttributeId s_kill_attribute_id = 0x0004;
static const int s_buffer_length = 64;

// Largest expected inbox and outbox message sizes
const uint32_t inbox_size = 64;
const uint32_t outbox_size = 256;

static void prv_update_text(void) {
  if (smartstrap_service_is_available(s_service_id)) {
    text_layer_set_text(s_status_layer, "Connected!");
  } else {
    text_layer_set_text(s_status_layer, "Connecting...");
  }
}

static void prv_send_score_update(uint32_t score, uint32_t myTeam) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "Sending score update. Score: %d ; myTeam: %d", score, myTeam);

  // Declare the dictionary's iterator
  DictionaryIterator *out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);

  if(result == APP_MSG_OK) {
    if (myTeam == 0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Score was reported, but myTeam was set to 0");
      return;
    } else if (myTeam == 1) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "%d", MESSAGE_KEY_RedScore);
      dict_write_int(out_iter, MESSAGE_KEY_RedScore, &score, sizeof(uint32_t), true);
    } else if (myTeam == 2) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "%d", MESSAGE_KEY_BlueScore);
      dict_write_int(out_iter, MESSAGE_KEY_BlueScore, &score, sizeof(uint32_t), true);
    } else {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Score was reported, but myTeam was set to %d", myTeam);
      return;
    }

    // Send this message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void prv_send_kill_update() {
  APP_LOG(APP_LOG_LEVEL_WARNING, "Sending kill update");

  // Declare the dictionary's iterator
  DictionaryIterator *out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);

  if(result == APP_MSG_OK) {
    int value = 0;
    dict_write_int(out_iter, MESSAGE_KEY_NewKill, &value, sizeof(int), true);

    // Send this message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void prv_did_read(SmartstrapAttribute *attr, SmartstrapResult result,
                              const uint8_t *data, size_t length) {
  if (attr == s_attr_attribute) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "did_read(s_attr_attribute, %d, %d)", result, length);
    if (result == SmartstrapResultOk && length == 4) {
      uint32_t num;
      memcpy(&num, data, 4);
      //snprintf(s_text_buffer1, 20, "%u", (unsigned int)num);
      //text_layer_set_text(s_attr_text_layer, s_text_buffer1);
    }
  } else if (attr == s_team_attribute) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "did_read(s_team_attribute, %d, %d)", result, length);
    if (result == SmartstrapResultOk) {
      uint32_t num;
      memcpy(&num, data, 4);
      //snprintf(s_text_buffer1, 20, "%u", (unsigned int)num);
      memcpy(&myTeam, data, 4);
      myTeam = num;
      if(num == 1) {
        window_set_background_color(s_main_window, GColorMelon);
      } else if(num == 2) {
        window_set_background_color(s_main_window, GColorCeleste);
      }
      text_layer_set_text(s_attr_text_layer, "Score:");

      snprintf(s_score_buffer, 20, "%u | %u", (unsigned int)redScore, (unsigned int)blueScore);
      text_layer_set_text(s_raw_text_layer, s_score_buffer);
      teamChosen = true;
    }
  } else if (attr == s_score_attribute) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "did_read(s_score_attribute, %d, %d)", result, length);
    if (result == SmartstrapResultOk) {
      uint32_t score;
      memcpy(&score, data, 4);
      if (myTeam == 1) {
        redScore += score;
      } else if (myTeam == 2) {
        blueScore += score;
      }
      snprintf(s_score_buffer, 20, "%u | %u", (unsigned int)redScore, (unsigned int)blueScore);
      text_layer_set_text(s_raw_text_layer, s_score_buffer);
      prv_send_score_update(score, myTeam);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "did_read(<%p>, %d)", attr, result);
  }
}

static void prv_did_write(SmartstrapAttribute *attr, SmartstrapResult result) {
  if (attr == s_attr_attribute) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "did_write(s_attr_attribute, %d)", result);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "did_write(<%p>, %d)", attr, result);
  }
}

static void prv_write_read_test_attr(void) {
  SmartstrapResult result;
  if (!smartstrap_service_is_available(smartstrap_attribute_get_service_id(s_attr_attribute))) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "service is not available");
    return;
  }

  /*
  // get the write buffer
  uint8_t *buffer = NULL;
  size_t length = 0;
  result = smartstrap_attribute_begin_write(s_attr_attribute, &buffer, &length);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Write of s_attr_attribute failed with result %d", result);
    return;
  }

  // write the data into the buffer
  uint32_t num = rand() % 10000;
  memcpy(buffer, &num, 4);

  // send it off
  result = smartstrap_attribute_end_write(s_attr_attribute, sizeof(num), true);
  if (result != SmartstrapResultOk) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Write of s_attr_attribute failed with result %d", result);
  }
  */
}

static void prv_send_request(void *context) {
  //prv_write_read_test_attr();
  //app_timer_register(900, prv_send_request, NULL);
}

static void prv_availablility_status_changed(SmartstrapServiceId service_id, bool is_available) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Availability for 0x%x is %d", service_id, is_available);
  prv_update_text();
}

static void prv_notified(SmartstrapAttribute *attr) {
  if (attr == s_attr_attribute) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "notified(s_attr_attribute)");
  } else if (attr == s_team_attribute) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "notified(s_team_attribute)");
    if(!teamChosen) {
      smartstrap_attribute_read(s_team_attribute);
    }
  } else if (attr == s_score_attribute) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "notified(s_score_attribute)");
    smartstrap_attribute_read(s_score_attribute);
  } else if (attr == s_kill_attribute) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "notified(s_kill_attribute)");
    prv_send_kill_update();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "notified(<%p>)", attr);
  }
}

static void prv_init_team(void) {
  smartstrap_attribute_read(s_team_attribute);
}

static void prv_main_window_load(Window *window) {
  s_status_layer = text_layer_create(GRect(0, 5, 144, 40));
  text_layer_set_font(s_status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  prv_update_text();
  text_layer_set_text_color(s_status_layer, GColorBlack);
  text_layer_set_background_color(s_status_layer, GColorClear);
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_status_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_status_layer));

  s_attr_text_layer = text_layer_create(GRect(0, 60, 144, 40));
  text_layer_set_font(s_attr_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text(s_attr_text_layer, "Select a team...");
  text_layer_set_text_color(s_attr_text_layer, GColorBlack);
  text_layer_set_background_color(s_attr_text_layer, GColorClear);
  text_layer_set_text_alignment(s_attr_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_attr_text_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_attr_text_layer));

  s_raw_text_layer = text_layer_create(GRect(0, 100, 144, 40));
  text_layer_set_font(s_raw_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text(s_raw_text_layer, "-");
  text_layer_set_text_color(s_raw_text_layer, GColorBlack);
  text_layer_set_background_color(s_raw_text_layer, GColorClear);
  text_layer_set_text_alignment(s_raw_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_raw_text_layer, GTextOverflowModeWordWrap);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_raw_text_layer));
}

static void prv_main_window_unload(Window *window) {
  text_layer_destroy(s_status_layer);
  text_layer_destroy(s_attr_text_layer);
  text_layer_destroy(s_raw_text_layer);
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  // A new message has been successfully received
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received message from phone");
  // Does this message contain a temperature value?
  Tuple *red_score_tuple = dict_find(iter, MESSAGE_KEY_RedScore);
  Tuple *blue_score_tuple = dict_find(iter, MESSAGE_KEY_BlueScore);
  Tuple *new_kill_tuple = dict_find(iter, MESSAGE_KEY_NewKill);
  if(red_score_tuple && blue_score_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)red_score_tuple->value->int32);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)blue_score_tuple->value->int32);
    snprintf(s_score_buffer, 20, "%d | %d", (int)red_score_tuple->value->int32, (int)blue_score_tuple->value->int32);
    text_layer_set_text(s_raw_text_layer, s_score_buffer);
  } else if (new_kill_tuple) {
    // Pointer to the attribute buffer
    size_t buff_size;
    uint8_t *buffer;

    // Begin the write request, getting the buffer and its length
    smartstrap_attribute_begin_write(s_kill_attribute, &buffer, &buff_size);

    // Store the data to be written to this attribute
    snprintf((char*)buffer, buff_size, "dummy");

    // End the write request, and send the data, not expecting a response
    smartstrap_attribute_end_write(s_kill_attribute, buff_size, false);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context) {
  // The message just sent has been successfully delivered

}

static void outbox_failed_callback(DictionaryIterator *iter,
                                          AppMessageResult reason, void *context) {
  // The message just sent failed to be delivered
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message send failed. Reason: %d", (int)reason);
}

static void prv_init(void) {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = prv_main_window_load,
    .unload = prv_main_window_unload
  });
  window_stack_push(s_main_window, true);
  SmartstrapHandlers handlers = (SmartstrapHandlers) {
    .availability_did_change = prv_availablility_status_changed,
    .did_write = prv_did_write,
    .did_read = prv_did_read,
    .notified = prv_notified
  };
  smartstrap_subscribe(handlers);
  smartstrap_set_timeout(50);
  s_attr_attribute = smartstrap_attribute_create(s_service_id, s_attr_attribute_id, s_buffer_length);
  s_team_attribute = smartstrap_attribute_create(s_service_id, s_team_attribute_id, s_buffer_length);
  s_score_attribute = smartstrap_attribute_create(s_service_id, s_score_attribute_id, s_buffer_length);
  s_kill_attribute = smartstrap_attribute_create(s_service_id, s_kill_attribute_id, s_buffer_length);
  //app_timer_register(1000, prv_send_request, NULL);
  prv_init_team();

  // Open AppMessage
  app_message_open(inbox_size, outbox_size);

  // Register to be notified about inbox received events
  app_message_register_inbox_received(inbox_received_callback);

  // Register to be notified about inbox dropped events
  app_message_register_inbox_dropped(inbox_dropped_callback);

  // Register to be notified about outbox sent events
  app_message_register_outbox_sent(outbox_sent_callback);

  // Register to be notified about outbox failed events
  app_message_register_outbox_failed(outbox_failed_callback);
}

static void prv_deinit(void) {
  window_destroy(s_main_window);
  smartstrap_unsubscribe();
}

int main(void) {
  prv_init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "STARTING APP");
  if (s_attr_attribute) {
    app_event_loop();
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ENDING APP");
  prv_deinit();
}

