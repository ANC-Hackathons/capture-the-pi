#include <ArduinoPebbleSerial.h>

// The buffer for transferring data
static uint8_t s_data_buffer[256];

static const uint16_t s_service_ids[] = {(uint16_t)0x1001};
static const uint16_t s_attr_ids[] = {(uint16_t)0x1001};

void setup() {
  Serial.begin(115200);

  int pin = 1;

  // Setup the Pebble smartstrap connection using one wire software serial
  ArduinoPebbleSerial::begin_software(pin, s_data_buffer, sizeof(s_data_buffer),
                                                   Baud57600, s_service_ids, 1);

  // Setup the Pebble smartstrap connection
  //ArduinoPebbleSerial::begin_hardware(s_data_buffer, sizeof(s_data_buffer),
  //                                                  Baud57600, s_service_ids, 1);
}

void loop() {
  if(ArduinoPebbleSerial::is_connected()) {
    Serial.println("Connection is valid");
    digitalWrite(13, LOW);
  } else {
    Serial.println("Connection is not valid");
    digitalWrite(13, HIGH);
  }
  delay(200);
}
