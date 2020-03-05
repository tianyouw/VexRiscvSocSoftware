import serial
import sys
import os
import time

# Get img as bytes
with open('images/pika.txt', 'r') as file:
    raw_data = file.read().rstrip("\r\n").replace('{', '').replace('}','')
raw_data = raw_data.split(',')

pixels = []
for s in raw_data:
    # pixels.append(int(s) & 0xFF)
    # pixels.append(int(s) >> 8)

    pixels.append(int(s) >> 11)             # blue
    pixels.append((int(s) & 0x7C0) >> 6)    # green (extra '>> 1' for color-correction)
    pixels.append(int(s) & 0x1F)            # red

byte_pixels = bytearray(pixels)

# for i in range(0, 4):
#     print(bin(pixels[i]))
# sys.exit()

# Open serial port
ser = serial.Serial()
ser.baudrate = 115200
ser.port = 'COM3' #replace with your port name
ser.bytesize = serial.EIGHTBITS
ser.stopbits = serial.STOPBITS_ONE
ser.parity = serial.PARITY_ODD
# ser.timeout = 10

print(ser)
ser.open()
if not ser.is_open:
    print("opening serial port failed")
    sys.exit()

# Write image to serial connection
# pls ignore buffer flushing...it doesn't work
ser.flush()
ser.reset_input_buffer()
ser.reset_output_buffer()
time.sleep(1)

ser.write(byte_pixels)
# ser.read()

ser.reset_input_buffer()
ser.reset_output_buffer()

ser.close()
