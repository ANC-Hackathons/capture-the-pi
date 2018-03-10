#include <ArduinoPebbleSerial.h>

// Pebble communication
static const uint16_t SUPPORTED_SERVICES[] = {0x0000, 0x1001};
static const uint8_t NUM_SERVICES = 2;
static uint8_t pebble_buffer[GET_PAYLOAD_BUFFER_SIZE(200)];

const int foreFingerPin = A2;     // The number of the palm reader pin
const int redLEDPin = 2;          // The number of the pin powering the red LEDs
const int whiteLEDPin = 3;        // The number of the pin powering the white LEDs
const int blueLEDPin = 9;         // The number of the pin powering the blue LEDs
const int forearmPin = 10;        // The number of the forearm pin
const uint8_t PEBBLE_PIN = 11;    // The number of the pin for serial communication with the Pebble

const int redBaseVolt = 0;        // Voltage of Red base (0 - 1023)
const int blueBaseVolt = 1023;     // Voltage of Blue base (0 - 1023)

const int redCarryVolt = 64;      // Voltage of Red player with flag (0 - 255)
const int deadArmVolt = 128;      // Voltage of arm without flag (0 - 255)
const int blueCarryVolt = 192;    // Voltage of Blue player with flag (0 - 255)

const int redCarryPulse = 510;    // Pulse for Red player with flag in ms
const int blueCarryPulse = 1530;  // Pulse for Blue player with flag in ms

const int debounceTime = 1000;    // The time in ms required for a switch to be stable

boolean teamChosen = false;       // Init sketch without a team
boolean blueTeam = false;
boolean redTeam = false;
boolean flagBearer = false;       // Init sketch as not flag bearer
boolean whiteToggle = false;      // Toggle for blinking the white LEDs

int score = 0;                    // Score tracker for testing

void setup() {
  // Set up serial communication
  Serial.begin(9600);

  // Initialize white LED pin
  pinMode(whiteLEDPin, OUTPUT);
  digitalWrite(whiteLEDPin, HIGH);

  // Initialize blue LED pin
  pinMode(blueLEDPin, OUTPUT);
  digitalWrite(blueLEDPin, LOW);

  // Initialize red LED pin
  pinMode(redLEDPin, OUTPUT);
  digitalWrite(redLEDPin, LOW);

  // Initialize forearm pin
  analogWrite(forearmPin, deadArmVolt);

  // Initialize falsearm pin for testing
  //analogWrite(falseArmPin, redCarryVolt);

  // Pebble connection monitor
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize Pebble connection
  STATIC_ASSERT_VALID_ONE_WIRE_SOFT_SERIAL_PIN(PEBBLE_PIN);
  ArduinoPebbleSerial::begin_software(PEBBLE_PIN, pebble_buffer, sizeof(pebble_buffer), Baud57600,
                                      SUPPORTED_SERVICES, NUM_SERVICES);
}

void loop() {
  static uint32_t last_check = 0;
  if (last_check == 0) {
    last_check = millis();
  }
  if (millis() - last_check > 100) {
    last_check = millis();
    if(!teamChosen) {
      // Initialize the glove team
      chooseTeam();
    } else {
      stealCheck();
      killCheck();
      captureCheck();

      if(flagBearer) {
        whiteToggle = !whiteToggle;
        digitalWrite(whiteLEDPin, whiteToggle);
      }

      Serial.println(score);
    }
  }
  
  // Let the ArduinoPebbleSerial code do its processing
  size_t length;
  uint16_t service_id;
  uint16_t attribute_id;
  RequestType type;
  if (ArduinoPebbleSerial::feed(&service_id, &attribute_id, &length, &type)) {
    if ((service_id == 0) && (attribute_id == 0)) {
      // we have a raw data frame to process
      static bool led_status = false;
      led_status = !led_status;
      digitalWrite(LED_BUILTIN, led_status);
      if (type == RequestTypeRead) {
        // send a response to the Pebble - reuse the same buffer for the response
        uint32_t current_time = millis();
        memcpy(pebble_buffer, &current_time, 4);
        ArduinoPebbleSerial::write(true, pebble_buffer, 4);
        Serial.println("Got raw data read");
      } else if (type == RequestTypeWrite) {
        Serial.print("Got raw data write: ");
        Serial.println((uint8_t)pebble_buffer[0], DEC);
      } else {
        // invalid request type - just ignore the request
      }
    } else if ((service_id == 0x1001) && (attribute_id == 0x1001)) {
      static uint32_t s_test_attr_data = 99999;
      if (type == RequestTypeWriteRead) {
        // read the previous value and write the new one
        uint32_t old_value = s_test_attr_data;
        memcpy(&s_test_attr_data, pebble_buffer, sizeof(s_test_attr_data));
        ArduinoPebbleSerial::write(true, (const uint8_t *)&old_value, sizeof(old_value));
        Serial.println("Got WriteRead for 0x1001,0x1001");
      } else {
        // invalid request type - just ignore the request
      }
    } else {
      // unsupported attribute - fail the request
      ArduinoPebbleSerial::write(false, NULL, 0);
    }
  }

  static bool is_connected = false;
  if (ArduinoPebbleSerial::is_connected()) {
    if (!is_connected) {
      Serial.println("Connected to the smartstrap!");
      is_connected = true;
    }
    static uint32_t last_notify = 0;
    if (last_notify == 0) {
      last_notify = millis();
    }
    // notify the pebble every 2.5 seconds
    if (millis() - last_notify  > 2500) {
      Serial.println("Sending notification for 0x1001,0x1001");
      ArduinoPebbleSerial::notify(0x1001, 0x1001);
      last_notify = millis();
    }
  } else {
    if (is_connected) {
      Serial.println("Disconnected from the smartstrap!");
      is_connected = false;
    }
  }
}

void chooseTeam() {
  int val = analogRead(foreFingerPin);
  Serial.println(val);
  if(val == redBaseVolt) {
    // Red base is kept low
    if (blockingDebounce(foreFingerPin, val)) {
      // Turn on the red LED
      digitalWrite(redLEDPin, HIGH);
      digitalWrite(whiteLEDPin, LOW);
      digitalWrite(blueLEDPin, LOW);
      teamChosen = true;
      redTeam = true;
    }
  //} else if (val == blueBaseVolt) {
  } else if (val >= blueBaseVolt && val <= blueBaseVolt + 10) {
    // Blue base is kept high
    if (blockingDebounce(foreFingerPin, val)) {
      // Turn on the blue LED
      digitalWrite(blueLEDPin, HIGH);
      digitalWrite(whiteLEDPin, LOW);
      digitalWrite(redLEDPin, LOW);
      teamChosen = true;
      blueTeam = true;
    }
  }
}

void stealCheck() {
  int val = analogRead(foreFingerPin);
  Serial.println(val);
  if (redTeam &&
      !flagBearer &&
      val >= blueBaseVolt - 10 &&
      val <= blueBaseVolt + 10 &&
      blockingDebounce(foreFingerPin, val)) {
    flagBearer = true;
  } else if (blueTeam &&
             !flagBearer &&
             val == redBaseVolt &&
             blockingDebounce(foreFingerPin, val)) {
    flagBearer = true;
  }
}

void killCheck() {
  int val = pulseIn(foreFingerPin, HIGH);
  //Serial.print("pulse: ");
  //Serial.println(val);
  if (redTeam && val >= blueCarryPulse - 15 && val <= blueCarryPulse + 15) {
    score++;
  } else if (blueTeam && val >= redCarryPulse - 15 && val <= redCarryPulse + 15) {
    score++;
  }
}

void captureCheck() {
  int val = analogRead(foreFingerPin);
  Serial.println(val);
  if (redTeam &&
      flagBearer &&
      val == redBaseVolt &&
      blockingDebounce(foreFingerPin, val)) {
    flagBearer = false;
    score += 5;
  } else if (blueTeam &&
             flagBearer && 
             val >= blueBaseVolt - 10 && 
             val <= blueBaseVolt + 10 && 
             blockingDebounce(foreFingerPin, val)) {
    flagBearer = false;
    score += 5;
  }
}

boolean blockingDebounce(int pin, int initialState) {
  Serial.println("Debouncing");
  int counter = 0;
  int state = initialState;

  //while (state == initialState) {  // Debounce loop
  while (state >= initialState - 50 && state <= initialState + 50) {  // Debounce loop
    if(counter >= debounceTime) {
      Serial.print("Returning true after ");
      Serial.print(counter);
      Serial.println("ms");
      return true;  // Return true if state hasn't changed
    }
    delay(1);
    counter++;
    state = analogRead(pin);
  }
  Serial.print("Returning false after ");
  Serial.print(counter);
  Serial.println("ms");
  return false;  // Return false if the state has changed
}
