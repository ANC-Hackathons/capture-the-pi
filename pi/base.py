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

    self.labelVariable = Tkinter.StringVar()
    self.label = Tkinter.Label(self, textvariable=self.labelVariable);
    self.label.grid(column=0, row=0, columnspan = 2);
    self.labelVariable.set(u"Select a Team:");

    self.buttonRed = Tkinter.Button(self, text=u"Red Base", command=self.setRed)
    self.buttonRed.grid(column=0, row=1);

    self.buttonBlue = Tkinter.Button(self, text=u"Blue Base", command=self.setBlue)
    self.buttonBlue.grid(column=1, row=1);

  def setRed(self):
    print 'Setting RED'
    GPIO.output(PIN_1, GPIO.LOW);
    GPIO.output(PIN_2, GPIO.HIGH);
    self.destroyTeamButtons();
    self.label.config(fg="white", bg='red')
    self.configure(background='red')
    self.labelVariable.set(u"Red Team\nRegister Players");

  def setBlue(self):
    print 'Setting BLUE'
    GPIO.output(PIN_1, GPIO.HIGH);
    GPIO.output(PIN_2, GPIO.LOW);
    self.destroyTeamButtons();
    self.label.config(fg="white", bg='blue')
    self.configure(background='blue')
    self.labelVariable.set(u"Blue Team\nRegister Players");

  def destroyTeamButtons(self):
    print 'Destroying Buttons'
    self.buttonRed.destroy()
    self.buttonBlue.destroy()
    self.buttonDone = Tkinter.Button(self, text=u"Done", command=self.doneRegistering)
    self.buttonDone.grid(column=0, row=1, columnspan=2);

  def doneRegistering(self):
    self.buttonDone.destroy()
    self.labelVariable.set(u"Waiting for other team...");

if __name__ == "__main__":
  PIN_1 = 26
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(PIN_1, GPIO.OUT)

  PIN_2 = 19
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(PIN_2, GPIO.OUT)

  app = app_tk(None)
  app.title('Capture The Pi')
  app.mainloop()
