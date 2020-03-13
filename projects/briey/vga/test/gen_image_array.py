#!/usr/bin/env python3
 
from PIL import Image
import sys

input_file = ""
 
def get_565(pixel):
    r, g, b = pixel
    r >>= 3
    g >>= 2
    b >>= 3
 
    return (b << 11) | (g << 5) | (r << 0)
 
def generate_struct(imagedata, width):
    imagedata = list(imagedata)
    return ',\n'.join(
            '{\n%s\n}' % (',\n'.join(
                '%d' % get_565(pixel) for pixel in imagedata[i:i+width]))
            for i in range(0, len(imagedata), width))
 
def generate_array(image):
    width, height = image.size

    imgfile = open(input_file.split('.')[0] + ".txt", "w")
    n = imgfile.write(generate_struct(image.getdata(), width))
    imgfile.close()

#     print("""#include <stdint.h>
 
# static uint16_t image[%d][%d] = {
# %s
# };""" % (width, height, generate_struct(image.getdata(), width)))

input_file = sys.argv[1]
generate_array(Image.open(input_file))