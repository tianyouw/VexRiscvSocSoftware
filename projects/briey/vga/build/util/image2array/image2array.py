#!/usr/bin/env python3

from PIL import Image
import sys

def generate_struct(imagedata, width):
    imagedata = list(imagedata)
    return ',\n'.join(
            '\t{\n%s\n\t}' % (',\n'.join(
                '\t\t{%d, %d, %d}' % pixel for pixel in imagedata[i:i+width]))
            for i in range(0, len(imagedata), width))

def generate_array(image):
    width, height = image.size
    print("""#include <stdint.h>

struct rgb {
\tuint8_t red;
\tuint8_t green;
\tuint8_t blue;
};

static struct rgb image[%d][%d] = {
%s
};""" % (width, height, generate_struct(image.getdata(), width)))

generate_array(Image.open(sys.argv[1]))
