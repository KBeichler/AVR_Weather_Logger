#!/usr/bin/env python

# uses https://github.com/TMRh20
# credit to J. Coliz <maniacbug@ymail.com>

from __future__ import print_function
import time
from RF24 import *

from struct import *

import RPi.GPIO as GPIO

irq_gpio_pin = None

########### USER CONFIGURATION ###########
# See https://github.com/TMRh20/RF24/blob/master/pyRF24/readme.md
# Radio CE Pin, CSN Pin, SPI Speed
# CE Pin uses GPIO number with BCM and SPIDEV drivers, other platforms use their own pin numbering
# CS Pin addresses the SPI bus number at /dev/spidev<a>.<b>
# ie: RF24 radio(<ce_pin>, <a>*10+<b>); spidev1.0 is 10, spidev1.1 is 11 etc..

# Generic:
radio = RF24(22,0);

#debug lvl
DEBUG = 1


pipes = [ "xxxx1" , "xxxx2" ]
max_payload_size = 32
rec_data_format = 'HHHHHHHHiIIHBB'



def try_read_data(channel=0):
    if radio.available():
		if DEBUG < 0:
			print ('available on channel '  + str(channel))
        while radio.available():

            receive_payload = radio.read(max_payload_size)
            
            if DEBUG > 1:
				for c in receive_payload :
					n = hex(c)
					n = n[2:]
					print(n + ' ', end='')
					
				print("")
            
            
            b_data = bytearray(receive_payload)
            data = unpack(rec_data_format , b_data)
            if DEBUG > 0:
				#debug print
				for ding in data:
					print(str(ding))
                
            radio.startListening()



radio.begin()
radio.setRetries(5, 15)
radio.setChannel(0)


if irq_gpio_pin is not None:
    # set up callback for irq pin
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(irq_gpio_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(irq_gpio_pin, GPIO.FALLING, callback=try_read_data)

radio.openWritingPipe(pipes[1])
radio.openReadingPipe(1, pipes[0])
radio.startListening()

if DEBUG < 0:
	radio.printDetails()

while 1:


        
        
	try_read_data(1)

		# callback routine set for irq pin takes care for reading -
		# do nothing, just sleeps in order not to burn cpu by looping
	time.sleep(5)
