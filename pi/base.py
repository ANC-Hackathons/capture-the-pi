#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import RPi.GPIO as GPIO

if __name__ == "__main__":
  PIN_1 = 26
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(PIN_1, GPIO.IN, pull_up_down=GPIO.PUD_UP)

  while True:
    if GPIO.input(PIN_1):
      print 'target 1 input was HIGH'
    else:
      print 'target 1 input was LOW'
