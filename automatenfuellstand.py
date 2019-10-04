#!/usr/bin/env python
# -*- coding: utf-8 -*-

import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

def distanceMeasurement(TRIG,ECHO):
    
    time.sleep(1)
    global pulseStart
    pulseStart = time.time()
    pulseEnd = time.time()
    pulseDuration = 0
    distance = 0
    GPIO.output(TRIG, True)
    time.sleep(0.001)
    GPIO.output(TRIG, False)

    while GPIO.input(ECHO) == 0:
        pulseStart = time.time()
    while GPIO.input(ECHO) == 1:
        pulseEnd = time.time()

    pulseDuration = pulseEnd - pulseStart
    distance = pulseDuration * 17150
    distance = round(distance, 2)
    return distance

#Configuration
GPIO_TRG_LIST = (14,8,24,4,27,18)
GPIO_ECH_LIST = (15,7,25,17,22,23)
GPIO.setup(GPIO_TRG_LIST,GPIO.OUT) #Trigger
GPIO.setup(GPIO_ECH_LIST,GPIO.IN)  #Echo

#Security
GPIO.output(14, False)
GPIO.output(8, False)
GPIO.output(24, False)
GPIO.output(4, False)
GPIO.output(27, False)
GPIO.output(18, False)
time.sleep(0.5)

#main Loop
try:
    while True:
       for i in range(6):
           if i == 0:
               recoveredDistance = distanceMeasurement(14,15)
               print "Distance1: ",recoveredDistance,"cm"
           elif i == 1:
               recoveredDistance = distanceMeasurement(8,7)
               print "Distance2: ",recoveredDistance,"cm"
           elif i == 2:
               recoveredDistance = distanceMeasurement(24,25)
               print "Distance3: ",recoveredDistance,"cm"
           elif i == 3:
               recoveredDistance = distanceMeasurement(4,17)
               print "Distance4: ",recoveredDistance,"cm"
           elif i == 4:
               recoveredDistance = distanceMeasurement(27,22)
               print "Distance5: ",recoveredDistance,"cm"
           elif i == 5:
               recoveredDistance = distanceMeasurement(18,23)
               print "Distance6: ",recoveredDistance,"cm"
       time.sleep(1)
except KeyboardInterrupt:
    print "Measurement stopped by user"
    GPIO.cleanup()
