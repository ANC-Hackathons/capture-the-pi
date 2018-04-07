# Pebble App Portion

The Pebble App portion relays scoring information from the glove-mounted Arduino to the player's smart phone in order to eventually persist it to the central scoring server. It displays a constantly updating game score and changes background color to indicate what team the player is on. It also monitors the serial connection between it and the Arduino and alerts the user if the connection is severed.

## Build application
```
pebble build
```

## Install application

* Connect your phone to the same wifi network as your computer
* Enable the [Developer Connection](https://developer.pebble.com/guides/tools-and-resources/developer-connection/) in the Pebble app on your phone
* Pass the Phone IP displayed in the app as an argument to the install command:
    ```
    pebble install --phone <PHONE_IP>
    ```

