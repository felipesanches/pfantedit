#!/usr/bin/env python

import string
import array

def palette(fp, addr):
	fp.seek(addr)
	status = fp.tell()
	pal = []
	for i in range(256):
		red = ord(fp.read(1))
		green = ord(fp.read(1))
		blue = ord(fp.read(1))
		pal.append( (red*256 + green)*256 + blue )
	print "Palette: ", status, "-", fp.tell(), "\n"
	return pal

code={}
def decodeimage(fp, file_offset, xsize, ysize, debug=False):
	global code
	image = array.array('B', " " * xsize * ysize)
#	if debug == True:
	if True:
		fp.seek(file_offset)
		code[file_offset]=[]
		for i in range(xsize*ysize):
			code[file_offset].append(ord(fp.read(1)))
		#print "encoded: "
		#print code[file_offset]

#	print "original code keys:", code.keys()
	
	fp.seek(file_offset)
	#states definition:
	START = 1
	COMPRESSED = 2
	RAW = 3
	
	state = START
	index = 0
	while index < xsize*ysize:
		if state == START:
			byte = ord(fp.read(1))
			if byte<=0x80:
				state = RAW
				count = byte + 1
			else:
				state=COMPRESSED
				count = 257 - byte
				color = ord(fp.read(1))
		elif state == COMPRESSED:
			if count == 0:
				state = START
			else:
				count = count - 1	
				image[index]=color
				index = index + 1
		elif state == RAW:
			if count == 0:
				state = START
			else:
				image[index] = ord(fp.read(1))
				index = index + 1
				count = count - 1
	print "image: ", file_offset, "-", fp.tell()
	if debug == True:
		print "raw :"
		print image

	return image

def decode_mask(fp, xsize, ysize, addr):
	mask = array.array('B', " " * xsize * ysize)
	xplot = 0
	yplot = 0
	fp.seek(addr)
	while(yplot < ysize):
		byte = ord(fp.read(1))
		j=128
		while j>0:
			mask[xsize*(yplot-1) + xplot -1] = (byte & j)/j
			xplot = xplot + 1
			if (xplot > xsize):
				xplot = 1
				yplot = yplot + 1
			j = j / 2
	print "mask: ", addr, "-", fp.tell()
	return mask

def load_sensors(fp, addr, n):
	fp.seek(addr)
	sensors = []
	a = array.array('B', " " * 8)
	while n>0:
		a[0]=ord(fp.read(1))
		a[1]=ord(fp.read(1))
		a[2]=ord(fp.read(1))
		a[3]=ord(fp.read(1))
		a[4]=ord(fp.read(1))
		a[5]=ord(fp.read(1))
		a[6]=ord(fp.read(1))
		a[7]=ord(fp.read(1))

		while a[1] > 1 or a[3] > 1 or a[5] > 1 or a[7] > 1:
			for i in range(6):
				a[i]=a[i+2]
			a[6]=ord(fp.read(1))
			a[7]=ord(fp.read(1))
		n=n-1
		sensors.append([a[0]+a[1]*256, a[2]+a[3]*256, a[4]+a[5]*256, a[6]+a[7]*256])
				
	return sensors
