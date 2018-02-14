const int palmPin = A2;           // The number of the palm reader pin
const int forearmPin = 11;        // The number of the forearm pin
const int falseArmPin = 10;       // The number of an extra pin for arm testing
const int whiteLEDPin = 2;        // The number of the pin powering the white LEDs
const int blueLEDPin = 3;         // The number of the pin powering the blue LEDs
const int redLEDPin = 9;          // The number of the pin powering the red LEDs

const int redBaseVolt = 0;        // Voltage of Red base (0 - 1023)
const int blueBaseVolt = 660;     // Voltage of Blue base (0 - 1023)
const int deadArmVolt = 128;      // Voltage of arm without flag (0 - 255)
const int redCarryVolt = 64;      // Voltage of Red player with flag (0 - 255)
const int redCarryPulse = 510;    // Pulse for Red player with flag in ms
const int blueCarryVolt = 192;    // Voltage of Blue player with flag (0 - 255)
const int blueCarryPulse = 1530;  // Pulse for Blue player with flag in ms
const int debounceTime = 3000;    // The time in ms required for a switch to be stable

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
  analogWrite(falseArmPin, redCarryVolt);
}

void loop() {
  // Initialize the glove team
  if(!teamChosen) {
    chooseTeam();
  } else {
    stealCheck();
    killCheck();

    if(flagBearer) {
      whiteToggle = !whiteToggle;
      digitalWrite(whiteLEDPin, whiteToggle);
    }

    Serial.println(score);
  }

  delay(100);
}

void chooseTeam() {
  int val = analogRead(palmPin);
  //Serial.println(val);
  if(val == redBaseVolt) {
    // Red base is kept low
    if (blockingDebounce(palmPin, val)) {
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
    if (blockingDebounce(palmPin, val)) {
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
  int val = analogRead(palmPin);
  //Serial.println(val);
  if (redTeam && !flagBearer && val == blueBaseVolt && blockingDebounce(palmPin, val)) {
    flagBearer = true;
  } else if (blueTeam && !flagBearer && val == redBaseVolt && blockingDebounce(palmPin, val)) {
    flagBearer = true;
  }
}

void killCheck() {
  int val = pulseIn(palmPin, HIGH);
  //Serial.print("pulse: ");
  //Serial.println(val);
  if (redTeam && val >= blueCarryPulse - 15 && val <= blueCarryPulse + 15) {
    score++;
  } else if (blueTeam && val >= redCarryPulse - 15 && val <= redCarryPulse + 15) {
    score++;
  }
}

void captureCheck() {
  
}

boolean blockingDebounce(int pin, int initialState) {
  Serial.println("Debouncing");
  int counter = 0;
  int state = initialState;

  //while (state == initialState) {  // Debounce loop
  while (state >= initialState - 20 && state <= initialState + 20) {  // Debounce loop
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
