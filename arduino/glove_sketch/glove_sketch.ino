#include <ArduinoPebbleSerial.h>

// Pebble communication
static const uint16_t s_service_ids[] = {(uint16_t)0x1001};
static const uint16_t s_attr_ids[] = {
  (uint16_t)0x0001,
  (uint16_t)0x0002,
  (uint16_t)0x0003,
  (uint16_t)0x0004
};
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

const int debounceTime = 2000;    // The time in ms required for a switch to be stable

boolean isTeamChosen = false;     // Init sketch without a team
boolean watchTeamSet = false;     // Flag for notifying Pebble of team selection
boolean blueTeam = false;
boolean redTeam = false;
boolean flagBearer = false;       // Init sketch as not flag bearer
boolean whiteToggle = false;      // Toggle for blinking the white LEDs

int score = 0;                    // Temporary local score tracker
boolean isScoreChanged = false;   // Flag for notifying Pebble of a score change
boolean canKill = true;           // Flag to prevent rapid extra kills

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
                                      s_service_ids, 1);
}

void loop() {

      //Serial.println(score);
  static uint32_t last_check = 0;
  if (last_check == 0) {
    last_check = millis();
  }
  if (millis() - last_check > 500) {
    last_check = millis();
    if(!isTeamChosen) {
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
    }
  }
  
  // Let the ArduinoPebbleSerial code do its processing
  size_t length;
  uint16_t service_id;
  uint16_t attribute_id;
  RequestType type;
  if (ArduinoPebbleSerial::feed(&service_id, &attribute_id, &length, &type)) {
    if ((service_id == s_service_ids[0]) && (attribute_id == s_attr_ids[0])) {
      static uint32_t s_test_attr_data = 99999;
      if (type == RequestTypeWriteRead) {
        // read the previous value and write the new one
        uint32_t old_value = s_test_attr_data;
        memcpy(&s_test_attr_data, pebble_buffer, sizeof(s_test_attr_data));
        ArduinoPebbleSerial::write(true, (const uint8_t *)&old_value, sizeof(old_value));
        Serial.println("Got WriteRead for 0x1001,0x0001");
      } else {
        // invalid request type - just ignore the request
      }
    } else if ((service_id == s_service_ids[0]) && (attribute_id == s_attr_ids[1])) {
      if (type == RequestTypeRead) {
        // s_attr_id[1] is a request for team. Respond with:
        //   0: unset
        //   1: red
        //   2: blue
        Serial.println("Got Read for 0x1001,0x0002");
        uint32_t team = 0;
        if(redTeam) {
          team = 1;
        } else if(blueTeam) {
          team = 2;
        }
        ArduinoPebbleSerial::write(true, (const uint8_t *)&team, sizeof(team));
        watchTeamSet = true;
      } else {
        // invalid request type - just ignore the request
      }
    } else if ((service_id == s_service_ids[0]) && (attribute_id == s_attr_ids[2])) {
      if (type == RequestTypeRead) {
        // s_attr_id[2] is a request for current local score
        Serial.println("Got Read for 0x1001,0x0003");
        ArduinoPebbleSerial::write(true, (const uint8_t *)&score, sizeof(score));
        score = 0;
        isScoreChanged = false;
      } else {
        // invalid request type - just ignore the request
      }
    } else if ((service_id == s_service_ids[0]) && (attribute_id == s_attr_ids[3])) {
      if (type == RequestTypeWrite) {
        // s_attr_id[2] is a request for current local score
        Serial.println("Got Write for 0x1001,0x0004");
        canKill = true;
      } else {
        // invalid request type - just ignore the request
      }
    } else {
      // unsupported attribute - fail the request
      ArduinoPebbleSerial::write(false, NULL, 0);
    }
  }

  // Update connection status
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
    // notify the pebble every second
    if (millis() - last_notify  > 1000) {
      Serial.println("Sending notification for 0x1001,0x0001");
      ArduinoPebbleSerial::notify(s_service_ids[0], s_attr_ids[0]);
      Serial.println(!watchTeamSet);
      Serial.println(isTeamChosen);
      if(isTeamChosen) {
        Serial.println("Sending notification for 0x1001,0x0002");
        ArduinoPebbleSerial::notify(s_service_ids[0], s_attr_ids[1]);
      }
      if(isScoreChanged) {
        Serial.println("Sending notification for 0x1001,0x0003");
        ArduinoPebbleSerial::notify(s_service_ids[0], s_attr_ids[2]);
      }
      if(!canKill) {
        Serial.println("Sending notification for 0x1001,0x0004");
        ArduinoPebbleSerial::notify(s_service_ids[0], s_attr_ids[3]);
      }
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
  if(val <= redBaseVolt + 50) {
    // Red base is kept low
    if (blockingDebounce(foreFingerPin, val)) {
      // Turn on the red LED
      digitalWrite(redLEDPin, HIGH);
      digitalWrite(whiteLEDPin, LOW);
      digitalWrite(blueLEDPin, LOW);
      isTeamChosen = true;
      redTeam = true;
    }
  } else if (val >= blueBaseVolt && val <= blueBaseVolt + 10) {
    // Blue base is kept high
    if (blockingDebounce(foreFingerPin, val)) {
      // Turn on the blue LED
      digitalWrite(blueLEDPin, HIGH);
      digitalWrite(whiteLEDPin, LOW);
      digitalWrite(redLEDPin, LOW);
      isTeamChosen = true;
      blueTeam = true;
    }
  }
}

void stealCheck() {
  int val = analogRead(foreFingerPin);
  if (redTeam &&
      !flagBearer &&
      val >= blueBaseVolt - 10 &&
      val <= blueBaseVolt + 10 &&
      blockingDebounce(foreFingerPin, val)) {
    flagBearer = true;
  } else if (blueTeam &&
             !flagBearer &&
             val <= redBaseVolt + 50 &&
             blockingDebounce(foreFingerPin, val)) {
    flagBearer = true;
  }
}

void killCheck() {
  int val = pulseIn(foreFingerPin, HIGH);
  if (canKill &&
      redTeam &&
      val >= blueCarryPulse - 15 &&
      val <= blueCarryPulse + 15 &&
      blockingPulseDebounce(foreFingerPin, val)) {
    score++;
    isScoreChanged = true;
    canKill = false;
  } else if (canKill &&
             blueTeam &&
             val >= redCarryPulse - 15 &&
             val <= redCarryPulse + 15 &&
             blockingPulseDebounce(foreFingerPin, val)) {
    score++;
    isScoreChanged = true;
    canKill = false;
  }
}

void captureCheck() {
  int val = analogRead(foreFingerPin);
  if (redTeam &&
      flagBearer &&
      val <= redBaseVolt + 50 &&
      blockingDebounce(foreFingerPin, val)) {
    flagBearer = false;
    score += 3;
    isScoreChanged = true;
    digitalWrite(whiteLEDPin, LOW);
  } else if (blueTeam &&
             flagBearer && 
             val >= blueBaseVolt - 10 && 
             val <= blueBaseVolt + 10 && 
             blockingDebounce(foreFingerPin, val)) {
    flagBearer = false;
    score += 3;
    isScoreChanged = true;
    digitalWrite(whiteLEDPin, LOW);
  }
}

boolean blockingDebounce(int pin, int initialState) {
  Serial.println("Debouncing");
  int counter = 0;
  int state = initialState;

  while (state >= initialState - 100 && state <= initialState + 100) {  // Debounce loop
    if(counter >= debounceTime) {
      return true;  // Return true if state hasn't changed
    }
    delay(1);
    counter++;
    state = analogRead(pin);
  }
  return false;  // Return false if the state has changed
}

boolean blockingPulseDebounce(int pin, int initialState) {
  Serial.println("Pulse Debouncing");
  int counter = 0;
  int state = initialState;

  while (state >= initialState - 100 && state <= initialState + 100) {  // Debounce loop
    if(counter >= debounceTime) {
      return true;  // Return true if state hasn't changed
    }
    delay(1);
    counter++;
    state = pulseIn(foreFingerPin, HIGH);
  }
  return false;  // Return false if the state has changed
}
