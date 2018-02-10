const int palmPin = 2;          // The number of the palm reader pin
const int whiteLEDPin = 2;      // The number of the pin powering the white LEDs
const int blueLEDPin = 3;       // The number of the pin powering the blue LEDs
const int redLEDPin = 9;        // The number of the pin powering the red LEDs

const int debounceTime = 3000;  // The time in ms required for a switch to be stable

boolean teamChosen = false;     // Init sketch without a team

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
}

void loop() {
  // Initialize the glove team
  while(!teamChosen) {
    delay(100);
    chooseTeam();
  }
}

void chooseTeam() {
  int val = analogRead(palmPin);
  Serial.println(val);
  if(val == 0) {
    // Red base is kept low
    if (blockingDebounce(palmPin, 0)) {
      // Turn on the red LED
      digitalWrite(redLEDPin, HIGH);
      digitalWrite(whiteLEDPin, LOW);
      digitalWrite(blueLEDPin, LOW);
      teamChosen = true;
    }
  } else if (val == 1023) {
    // Blue base is kept high
    if (blockingDebounce(palmPin, 1023)) {
      // Turn on the blue LED
      digitalWrite(blueLEDPin, HIGH);
      digitalWrite(whiteLEDPin, LOW);
      digitalWrite(redLEDPin, LOW);
      teamChosen = true;
    }
  }
}

boolean blockingDebounce(int pin, int initialState) {
  Serial.println("Debouncing");
  int counter = 0;
  int state = initialState;

  while (state == initialState) {  // Debounce loop
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
