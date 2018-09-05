# Capture The Pi
Capture The Pi is a digital take on the classic Capture The Flag game.

<img src="https://i.imgur.com/OtffugQ.jpg" height="360">

## How it Works

Each player wears a glove with a [LilyPad Arduino](https://www.sparkfun.com/products/12049) sewn into it. The pins of the arduino connects to conductive pads in the tips of the thumb, forefinger, and middle finger that are capable of reading voltage inputs. The glove is able to steal the flag from the opposing team's base, which is composed of a Raspbery Pi, by making physical contact with conductive pads attached to specific GPIO pins. Returning the opposing team's flag to your own base scores points for your team. However, while you carry the flag, a specific voltage pulse is sent to conductive pads on your forearm. If the opposing team grabs you while you have their flag, they return their flag and score points for their team. LEDs that are also sewn into the forearm denote what team you're on, as well as if you currently have the flag. Score is kept in a centralized Node.js server. The Python app running in the Raspberry Pi base station continuously pings the centralized server to display updated scoring information. The Arduino in the glove communicates scoring info to a Pebble Time smartwatch warn on the player's wrist via single-pin software IO. The Pebble Time relays information to and from an app on the player's Android smartphone, which in turn is also able to ping the central scoring server.

## Running the Project

This project is split into mutliple components. Specific information regarding installing and running each component can be found in the README's of the corresponding directories. The [notes](https://github.com/thompsnm/capture-the-pi/tree/master/notes) directory contains some information about Arduino/Pebble serial communication, as well as diagrams for building the gloves yourself.

## Accolades

Capture The Pi earned the title of Most Creative during the 2018 ReadyTalk Pi Day Hackathon. A demonstration of the project can be seen on YouTube:

[![Capture The Pi presentation](https://img.youtube.com/vi/7TkEQ0ZJv1I/0.jpg)](https://www.youtube.com/embed/7TkEQ0ZJv1I?rel=0)
