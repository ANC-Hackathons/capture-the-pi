# Arduino Sketch Portion

The Arduino sketch is responsible for all glove controls. This includes reading voltages, outputing voltage pulses, and controling LEDs strips. As such, it is the only portion of the app that can determine when the score for a team needs to be incremented. It must then relay this information to the Pebble Time so that it can eventually be persisted to the 

## Verify / Compile the Sketch

* Download the [Arduino IDE](https://www.arduino.cc/en/Main/Software)
* From the Arduino IDE, select Tools -> Board -> LilyPad Ardunio USB
* Click the Verify button in the Arduino IDE

## Upload the Sketch

* Follow the steps to Verify / Compile the sketch
* Connect a LilyPad Ardunio USB to your computer via a USB cable capable of transfering data. Ensure that the power switch for the Arduino is set to On, not Charge.
* From the Arduino IDE, select Tools -> Port -> the USB port where your Arduino is plugged in
    * **Troubleshooting:** If you don't see your port, you may be using a cable that is only capable of transfering power, not data. Try with another cable. A cable that came with a cell phone is likely to work.
* Click the Upload button in the Arduino IDE

## Build the Circuits

See the diagrams in the [notes](https://github.com/thompsnm/capture-the-pi/tree/master/notes) directory to identify which pins control which components.

## Credits

The code to enable serial communication with a Pebble device is heavily based on the [ArduinoPebbleSerial](https://github.com/pebble/ArduinoPebbleSerial) project provided by the team at Pebble.
