#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
* kame-hame-ra takes timelapses and foto controlled by mobile OSC
* displays info in i2c OLED 
* controlled via android OSC app.
* coolness
"""

import cv2, time
import subprocess
import Adafruit_SSD1306

from picamera.array import PiRGBArray
from picamera import PiCamera

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

# Raspberry Pi pin configuration:
RST = None

# 128x32 display with hardware I2C:
disp = Adafruit_SSD1306.SSD1306_128_32(rst=RST)
disp.begin()
disp.clear()
disp.display()

# Create blank image for drawing. Mode '1' for 1-bit color.
width = disp.width
height = disp.height
image = Image.new('1', (width, height))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)

# Draw a black filled box to clear the image.
draw.rectangle((0,0,width,height), outline=0, fill=0)

padding = -2
top = padding
bottom = height-padding
x = 1

# Load no default font.
font = ImageFont.truetype('Symtext.ttf', 8)

#--- ##
if __name__ == "__main__":
	camera = PiCamera()
	print "[_OverCam_]"
	camera.resolution = (1920, 1080)
	#camera.resolution = (2592, 1944)

	for i in range(-6,1080):
		camera.capture('./eclipse01/img_%04d.jpg'%i)
		#cv2.imwrite('img_'+str(i)+'.png', gImg)
		print (i)

		# Draw a black filled box to clear the image.
	    draw.rectangle((1, 1, width-1, height-1), outline=1, fill=0)
		# Shell scripts for system monitoring from here : https://unix.stackexchange.com/questions/119126/command-to-display-memory-usage-disk-usage-and-cpu-load
		cmd = "hostname -I | cut -d\' \' -f1"
		IP = subprocess.check_output(cmd, shell = True )
		cmd = "top -bn1 | grep load | awk '{printf \"CPU Load: %.2f\", $(NF-2)}'"
		CPU = subprocess.check_output(cmd, shell = True )
		cmd = "free -m | awk 'NR==2{printf \"Mem: %s/%sMB %.2f%%\", $3,$2,$3*100/$2 }'"
		MemUsage = subprocess.check_output(cmd, shell = True )
		cmd = "df -h | awk '$NF==\"/\"{printf \"Disk: %d/%dGB %s\", $3,$2,$5}'"
		Disk = subprocess.check_output(cmd, shell = True )
		# Write four lines of text.
		draw.text((x, top),       "OverCam_v1: " + str(IP),  font=font, fill=255)
		draw.text((x, top+8),     "Writing to: " + str(dire), font=font, fill=255)
		draw.text((x, top+16),    "Photo: " + str(i) + " / " + str(i),  font=font, fill=255)
		draw.text((x, top+25),    str(Disk),  font=font, fill=255)

		# rest some time before next photo
		time.sleep(10)
	print "done"
