import time
import machine

#pin = machine.Pin(14, machine.Pin.OUT)   #D5 pin

#pin1 = machine.Pin(2, machine.Pin.OUT)    #Built-in LED
pin = machine.Pin(16, machine.Pin.OUT)   #Built-in LED

while True:
    pin.on() 
    time.sleep(1)
    pin.off()
    time.sleep(1)
