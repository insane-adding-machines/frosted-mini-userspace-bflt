#!/usr/bin/python
import serial
s = serial.Serial("/dev/ttyUSB0", baudrate=115200)
s.write('x')
s.close()
