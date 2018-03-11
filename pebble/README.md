# Pebble App Portion

## Pair your computer with your Pebble Time
* Enable Bluetooth is on your Pebble Time
* Ensure your Pebble Time is not paired with another device. You may need to disable Bluetooth on your phone.
* Enable Bluetooth on your computer
* Pair your computer with your watch as you would with any other device. On OSX the process would be as follows:
    * Open System Preferences
    * Click Bluetooth
    * Click "Turn Bluetooth On" if it wasn't on already
    * Select your Pebble Time from the list. It will look something like "Pebble Time XXXX", where XXXX is the unique identifier for your watch.
    * Click the Pair button
        * NOTE: After pairing, your Pebble Time may still say "Not Connected". This is fine. The pairing process will still have created the necessary device file.
* Find the device file for your Pebble Time on your computer. On OS X, this is similar to `/dev/cu.PebbleTimeXXXX-SerialPo` or `/dev/cu.PebbleXXXX-SerialPortSe`

## Build application
```
pebble build
```

## Install application
```
pebble install --serial <PEBBLE_TIME_DEVICE_FILE>
```

