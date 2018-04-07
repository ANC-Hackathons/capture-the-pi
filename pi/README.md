# Raspberry Pi Portion

The Raspberry Pi portion acts as a home base for each team. On startup it displays a Tkinter app that allows players to designate a particular team. Once that is complete, it will set the voltage output on pin 26 to either high or low. Players can then touch the pin with the forefinger of their glove while also touching a ground pin to set their team. It's recommended you attach some sort of conductive pad to each pin for ease of use. Once the game begins, the Tkinter app displays a constantly updating score, and the same pin configuration is used to steal and capture the opponent's flag.

## Install Dependencies

    pip install -r requirements.txt
 
## Run App

    ./base.py
