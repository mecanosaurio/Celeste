#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
* kame-hame-ra takes timelapses and foto controlled by mobile OSC
* displays info in i2c OLED 
* controlled via android OSC app.
* coolness

v0.1 180704
receives osc for a time, then keeps on st0
v0.2 180925
better resolution, also buttonable start
"""

import time, datetime, os
import subprocess
import Adafruit_SSD1306
import types

import OSC, threading, socket, sys
import subprocess

from picamera import PiCamera
import RPi.GPIO as GPIO

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

def handle_timeout(self):
	self.timed_out = True

def cam_start_callback(addr, tags, args, source):
	global run, state
	if args[0]==1:
		state = 1
		run = True
		print "[overcam]: --->]"

def cam_stop_callback(path, tags, args, source):
	global run, state
	if args[0]==1:
		state = 0
		run = False
		print "[overcam]: ---x]"

def other_start_callback():
	global run, state
	state = 1
	run = True
	print "[overcam]: --->]"

def other_stop_callback():
	global run, state
	state = 0
	run = False
	print "[overcam]: ---x]"

def each_frame():
    # clear timed_out flag
    s.timed_out = False
    # handle all pending requests then return
    while not s.timed_out:
        s.handle_request()


#--- ##
if __name__ == "__main__":
	#- init camera
	#print(GPIO.RPI_INFO)
	camera = PiCamera()
	print "[_OverCam_]"
	#- Raspberry Pi pin configuration:
	RST = None
	#- 128x32 display with hardware I2C:
	disp = Adafruit_SSD1306.SSD1306_128_32(rst=RST)
	disp.begin()
	disp.clear()
	disp.display()
	
	#GPIO.setmode(GPIO.BOARD)
	GPIO.setup(22, GPIO.IN)
	
	#- Create blank image for drawing. Mode '1' for 1-bit color.
	width = disp.width
	height = disp.height
	image = Image.new('1', (width, height))
	draw = ImageDraw.Draw(image)
	draw.rectangle((0,0,width,height), outline=0, fill=0)
	padding = -2
	top = padding
	bottom = height-padding
	x = 1
	#- load no default font.
	font = ImageFont.truetype('/home/pi/W/kamera/Symtext.ttf', 8)
	#- osc inits
	cmd = "hostname -I | cut -d\' \' -f1"
	IP = subprocess.check_output(cmd, shell = True )
	serv_ip = IP.strip().rstrip()
	s = OSC.OSCServer((serv_ip, 17110))
	s.timeout = 0
	run = True
	#s.handle_timeout = types.MethodType(handle_timeout, s)
	# adding callback functions to listener
	s.addDefaultHandlers()
	s.addMsgHandler("/kame/start", cam_start_callback)
	s.addMsgHandler("/kame/stop", cam_stop_callback)
	# OSCServer in thread
	st = threading.Thread( target = s.serve_forever )
	#st.daemon = True
	st.start()
	print "[overcam.ip]: "+serv_ip

	#- other inits
	base_dir = "/media/ship/"
	act_dir = ""
	state = 0

	camera.resolution = (2592, 1944)
	#camera.resolution = (1920, 1080)
	npics = 2160

	while True:
		st_btn = GPIO.input(22)
		#print st_btn
		if state==0:			# st0 is waiting
			#print "[overcam.nil] -"
			# check bttn
			if (not st_btn): 
				other_start_callback()
				#st_btn = 1
				time.sleep(1)
			#- get info
			cmd = "hostname -I | cut -d\' \' -f1"
			IP = subprocess.check_output(cmd, shell = True )
			IP = IP.strip().rstrip()
			cmd = "df -h | awk '$NF==\"/media/ship\"{printf \"Disk: %d/%dGB %s\", $3,$2,$5}'"
			Disk = subprocess.check_output(cmd, shell = True )
			ta = datetime.date.today().isoformat().replace("-","")
			tb = time.asctime()[11:16].replace(":","")
			Now = ta+"_"+tb
			#- clean draw
			draw.rectangle((1, 1, width-1, height-1), outline=0, fill=0)
			#- write text
			draw.text((x, top),		" OverCam  =" + str(IP)+"=",  font=font, fill=255)
			draw.text((x, top+17),	" t: " + Now, font=font, fill=255)
			draw.text((x, top+25),	" o: " + "/kame/start ON",  font=font, fill=255)
			#- display image
			disp.image(image)
			disp.display()
			time.sleep(.1)
			#- run the osc jobs
			##each_frame()

		if state==1:			#st1 is photoing

			print "[overcam.timelapse] ---"
			#- get new data
			ta = datetime.date.today().isoformat().replace("-","")
			tb = time.asctime()[11:16].replace(":","")
			Now = ta+"_"+tb

			#- checj dir
			act_dir = base_dir + Now
			if not os.path.exists(act_dir):
				os.makedirs(act_dir)
				print "[overcam.dir] : "+act_dir+'/'

			#- make the pics
			print "[photo.#] "
			for i in range(-3, npics):
				#- makep ohoto
				camera.capture(act_dir+'/img_%04d.jpg'%i)
				print ":"+str(i),
				#- get info
				cmd = "hostname -I | cut -d\' \' -f1"
				IP = subprocess.check_output(cmd, shell = True )
				IP = IP.strip().rstrip()
				cmd = "top -bn1 | grep load | awk '{printf \"CPU Load: %.2f\", $(NF-2)}'"
				CPU = subprocess.check_output(cmd, shell = True )
				cmd = "free -m | awk 'NR==2{printf \"Mem: %s/%sMB %.2f%%\", $3,$2,$3*100/$2 }'"
				MemUsage = subprocess.check_output(cmd, shell = True )
				cmd = "df -h | awk '$NF==\"/media/ship\"{printf \"Disk: %d/%dGB %s\", $3,$2,$5}'"
				#cmd = "df -h | awk '$NF==\"/\"{printf \"Disk: %d/%dGB %s\", $3,$2,$5}'"
				Disk = subprocess.check_output(cmd, shell = True )

				#- clean draw
				draw.rectangle((1, 1, width-1, height-1), outline=0, fill=0)
				# Write four lines of text.
				draw.text((x, top),		" OverCam  =" + str(IP)+"=",  font=font, fill=255)
				draw.text((x, top+8),	" d: " + str(act_dir[6:]), font=font, fill=255)
				draw.text((x, top+17),	" #: " + str(i) + " / " + str(npics),  font=font, fill=255)
				draw.text((x, top+25),	" s: " + str(Disk),  font=font, fill=255)
				#- display image
				disp.image(image)
				disp.display()
				# rest some time before next photo
				for j in range(5):
					##each_frame()
					time.sleep(1)
			print "\n[overcam.lapse] : "+act_dir+' w/'+str(npics)
			state = 0
			#s.timeout = 0
	s.close()
	GPIO.cleanup()
	draw.rectangle((1, 1, width-1, height-1), outline=0, fill=0)
	draw.text((x, top+8),	" <- SLEEP SINCE -> ",  font=font, fill=255)
	ta = datetime.date.today().isoformat().replace("-","")
	tb = time.asctime()[11:16].replace(":","")
	Now = ta+"_"+tb
	draw.text((x, top+17),	"  " + Now, font=font, fill=255)
	disp.image(image)
	disp.display()

