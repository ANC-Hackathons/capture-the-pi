#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import RPi.GPIO as GPIO
import Tkinter
import time
import datetime

class player_frame(Tkinter.Frame):
  def __init__(self, parent):
    Tkinter.Frame.__init__(self, parent)
    self.parent = parent
    self.initialize()

  def initialize(self):

    self.TARGET_1 = 6
    self.TARGET_2 = 13
    self.TARGET_3 = 19
    self.TARGET_4 = 26

    self.grid()

    buttonClock = Tkinter.Button(self, text=u"Start Clock", command=self.OnStartClock)
    buttonClock.grid(column=2, row=0)

    self.labelVariable = Tkinter.StringVar()
    label = Tkinter.Label(self, textvariable=self.labelVariable, anchor="w", fg="white", bg="blue")
    label.grid(column=0, row=0, sticky='EW')
    self.labelVariable.set(self.parent.entryVariable.get())

    self.timer = Tkinter.StringVar()
    label = Tkinter.Label(self, textvariable=self.timer, padx=10, fg="white", bg="black")
    label.grid(column=1, row=0, sticky='EW')
    self.timer.set(u"00:00.000")

    self.grid_columnconfigure(0, weight=1)
#    self.grid_columnconfigure(1, weight=1)

  def OnStartClock(self):
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(self.TARGET_1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(self.TARGET_2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(self.TARGET_3, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(self.TARGET_4, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    start_time = time.time()
    print 'Started timer'
    while GPIO.input(self.TARGET_1) == GPIO.HIGH or \
          GPIO.input(self.TARGET_2) == GPIO.HIGH or \
          GPIO.input(self.TARGET_3) == GPIO.HIGH or \
          GPIO.input(self.TARGET_4) == GPIO.HIGH:

      time.sleep(0.2)

      timeDisplay = datetime.datetime.fromtimestamp(time.time() - start_time).strftime('%M:%S.%f')[:-3]
      self.timer.set(timeDisplay)
      self.update()

      if GPIO.input(self.TARGET_1):
        print 'target 1 input was HIGH'
      else:
        print 'target 1 input was LOW'

      if GPIO.input(self.TARGET_2):
        print 'target 2 input was HIGH'
      else:
        print 'target 2 input was LOW'

      if GPIO.input(self.TARGET_3):
        print 'target 3 input was HIGH'
      else:
        print 'target 3 input was LOW'

      if GPIO.input(self.TARGET_4):
        print 'target 4 input was HIGH'
      else:
        print 'target 4 input was LOW'

    end_time = time.time()
    print 'Stopped time'
    print 'Time: ' + str(end_time - start_time)
    GPIO.cleanup()

class app_tk(Tkinter.Tk):
  def __init__(self, parent):
    Tkinter.Tk.__init__(self, parent)
    self.parent = parent
    self.initialize()

  def initialize(self):
    self.grid()

    self.currentPlayerRow = 1
    self.currentPlayerNumber = 1

    self.entryVariable = Tkinter.StringVar()
    self.entry = Tkinter.Entry(self, textvariable=self.entryVariable)
    self.entry.grid(column=0, row=0, sticky='EW')
    self.entry.bind("<Return>", self.OnPressEnter)
    self.entryVariable.set(u"Player " + str(self.currentPlayerNumber))

    buttonName = Tkinter.Button(self, text=u"Add Name", command=self.OnAddName)
    buttonName.grid(column=1, row=0)

    self.grid_columnconfigure(0, weight=1)
    self.update()
    self.geometry(self.geometry())
    self.entry.focus_set()
    self.entry.selection_range(0, Tkinter.END)

  def OnAddName(self):
#    self.labelVariable.set(self.entryVariable.get())
    newPlayerRow = player_frame(self)
    newPlayerRow.grid(column=0, row=self.currentPlayerRow, sticky='EW')
    self.currentPlayerRow += 1
    self.currentPlayerNumber += 1

    self.entryVariable.set(u"Player " + str(self.currentPlayerNumber))
    self.entry.focus_set()
    self.entry.selection_range(0, Tkinter.END)

  def OnPressEnter(self, event):
    self.OnAddName()
#    self.labelVariable.set(self.entryVariable.get() + " (You pressed enter !)")
#    self.entry.focus_set()
#    self.entry.selection_range(0, Tkinter.END)

if __name__ == "__main__":
  app = app_tk(None)
  app.title('Pi-cision Shooting')
  app.mainloop()

#if GPIO.input(TARGET_1):
#  print 'input was HIGH'
#else:
#  print 'input was LOW'

#if GPIO.input(TARGET_2):
#  print 'input was HIGH'
#else:
#  print 'input was LOW'

#if GPIO.input(TARGET_3):
#  print 'input was HIGH'
#else:
#  print 'input was LOW'

#if GPIO.input(TARGET_4):
#  print 'input was HIGH'
#else:
#  print 'input was LOW'
