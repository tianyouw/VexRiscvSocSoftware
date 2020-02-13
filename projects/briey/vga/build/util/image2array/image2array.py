#!/usr/bin/env python3

from PIL import Image
import sys

def convert_888_to_565(pixel):
    red, green, blue = pixel
    red >>= 3
    green >>= 2
    blue >>= 3

    return (red << 11) | (green << 5) | (blue << 0)

def generate_struct(imagedata, width):
    imagedata = list(imagedata)
    return ',\n'.join(
            '\t{\n%s\n\t}' % (',\n'.join(
                '\t\t0x%04x' % convert_888_to_565(pixel) for pixel in imagedata[i:i+width]))
            for i in range(0, len(imagedata), width))

def generate_array(image):
    width, height = image.size
    print("""#include <stdint.h>

static uint16_t image[%d][%d] = {
%s
};""" % (width, height, generate_struct(image.getdata(), width)))

generate_array(Image.open(sys.argv[1]))
