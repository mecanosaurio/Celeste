#!/usr/bin/env python
# -*- coding: utf-8 -*-

from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2, time

##  --- ----- --- ----- --- ----- ---- ------ ---- --- - -- --- - - -- - - ##
if __name__ == "__main__":
	camera = PiCamera()
	print "cam OK"
	camera.resolution = (1920, 1080)
	for i in range(-6,1080):
		camera.capture('./eclipse01/img_%04d.jpg'%i)
		#cv2.imwrite('img_'+str(i)+'.png', gImg)
		print (i)
		time.sleep(10)
	print "done"

