import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
hold = 24
GPIO.setup(hold, GPIO.OUT)
GPIO.setup(23, GPIO.OUT)
holdstate = GPIO.input(hold)
if holdstate:
    print("stopped holding")
else:
    print("started holding")
GPIO.output(hold, not holdstate)
GPIO.output(23, not holdstate)
