#!/usr/bin/env python

import string
import array
import os

bounds = None
current_addr = 0
max_addr = 0

def palette(fd, addr, pal):
	os.lseek(fd, addr, 0)
	for i in range(len(pal)):
		r,g,b = pal[i]
		os.write(fd, chr(r))
		os.write(fd, chr(g))
		os.write(fd, chr(b))

def write(fd,val):
	global bounds, current_addr, max_addr
	if bounds == None:
		bounds = [current_addr,current_addr]
	else:
		bounds = [min(bounds[0], current_addr), max(bounds[0], current_addr)]

	if current_addr <= max_addr:
		os.write(fd,val)
	current_addr+=1

def seek(fd,addr):
	global current_addr, bounds
	bounds = None
	current_addr = addr
	os.lseek(fd, addr, 0)

def save_image(fd, image, addr, maxaddr, xsize, ysize):
	global max_addr
	max_addr = maxaddr
	seek(fd, addr)
	
	pixels = []

	#states definitions
	START = 0
	PAIR = 1
	COMPRESSION = 2
	RAW = 3

	state = START
	pixels.append(image[0])
	index = 1
	while(index<xsize*ysize):
		if state == START:
			pair_in_raw = False
			if image[index] == pixels[0]:
				state = COMPRESSION
				pixels = pixels[0]
			else:
				state = RAW
				pixels.append(image[index])
			count = 2
			index += 1			
		elif state == PAIR:
			if image[index] == pixels[1]:
				state = COMPRESSION
				pixels = pixels[0]
			else:
				state = RAW
				pixels.append(image[index])
			count = 3
			index += 1
		elif state == COMPRESSION:
			if image[index] == pixels:
				#limits size of the segment being compressed
				#cannot exceed the maximum size of 0x80
				if count == 0x80: #(?)verificar
					write(fd, chr(0x81))
					write(fd, chr(pixels))
					state = START
					pixels = [image[index]]
					index += 1
				else: # segment to be compressed continue growing
					count += 1
					index += 1
			else:
				#record the compression
				write(fd, chr(257-count))
				write(fd, chr(pixels))
				#and go back to the initial state
				state = START
				pixels = [image[index]]
				index += 1
		elif state == RAW:
			#limit the size of the raw data segment to 
			#the maximum lenght of 0x81 pixels
			if count == 0x81:
				#record the raw data
				write(fd, chr(count-1))
				for pixel in pixels:
					write(fd, chr(pixel))
				#and go back to the initial state
				state = START
				pixels = [image[index]]
				index += 1
			else: #continue to append raw data
				if image[index] == pixels[count-1]:
					if pair_in_raw is True:
						state = PAIR
						write(fd, chr(count-3)) # this represents
								# a count-2 raw data segment
						for i in range(count-2):
							write(fd, chr(pixels[i]))
						pixels = []
						pixels.append(image[index])
						pixels.append(image[index])
					else:
						pair_in_raw = True
						pixels.append(image[index])
						index += 1
						count += 1
				else:
					pair_in_raw = False
					pixels.append(image[index])
					index += 1
					count += 1

	print "bounds: ", bounds

	import loadprg
	print "comparando codigo inicial e final:"
	os.lseek(fd, addr, 0)
	for i in range(xsize*ysize):
		if loadprg.code[addr][i] != ord(os.read(fd, 1)):
			os.lseek(fd, addr+i-10, 0)
			print "indice: ", i
			for pos in range(i-10,i+10):
				print loadprg.code[addr][pos], "---", ord(os.read(fd, 1))
			return

