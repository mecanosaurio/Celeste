#!/usr/bin/env python
# -*- coding: utf-8 -*-

from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2, time

##  --- ----- --- ----- --- ----- ---- ------ ---- --- - -- --- - - -- - - ##
if __name__ == "__main__":
	with PiCamera() as cam:
		cam.resolution = (1280, 720)
		cam.framerate = 10
		cam.contrast = 100
		cam.brightness = 20		
		#cam.start_preview()
		time.sleep(2)
		stream = PiRGBArray(cam)
		i=0
		while (i<10):
			try:
					actImg = stream.array
					#gImg = cv2.cvtColor(actImg,cv2.COLOR_BGR2GRAY)
					#img = cv2.resize(gImg, (0,0), fx=1.0, fy=1.0)
			except:
					print "ouch"
					continue
																	# preprocess
			cv2.imwrite('img_%04d.jpg' % i, actImg)
			print (i)
			i+=1
			time.sleep(5)
		print "done"
		
