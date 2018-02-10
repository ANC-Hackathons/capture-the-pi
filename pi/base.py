#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import RPi.GPIO as GPIO
import Tkinter
import time

class app_tk(Tkinter.Tk):
  def __init__(self, parent):
    Tkinter.Tk.__init__(self,parent)
    self.parent = parent
    self.initialize()

  def initialize(self):
    self.grid()

    buttonRed = Tkinter.Button(self, text=u"Red Base", command=self.setRed)
    buttonRed.grid(column=0, row=0);

    buttonGreen = Tkinter.Button(self, text=u"Green Base", command=self.setGreen)
    buttonGreen.grid(column=1, row=0);

  def setRed(self):
    print 'Setting RED'
    GPIO.output(PIN_1, GPIO.LOW);

  def setGreen(self):
    print 'Setting GREEN'
    GPIO.output(PIN_1, GPIO.HIGH);

if __name__ == "__main__":
  PIN_1 = 26
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(PIN_1, GPIO.OUT)


  app = app_tk(None)
  app.title('Capture The Pi')
  app.mainloop()
