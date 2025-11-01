import OPi.GPIO as GPIO 
from time import sleep 
GPIO.setboard(GPIO.H616)
GPIO.setwarnings(False) 
 
GPIO.setmode(GPIO.BCM) 
GPIO.setup(11, GPIO.OUT) 
GPIO.output(11, True) 
sleep(2) 
GPIO.output(11, False)
