#!/usr/bin/env python
# -*- coding: cp1252 -*-

'''
/* Copyright (C) 2017, Attila Kov�cs
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation  and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of any
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

 This script converts all png and bmp files to files compatible with loading to TFT
 screens by microcontrollers. These files are then concatenated into a single c file.
 '''

import glob
import re
import sys
import subprocess
from pprint import pprint
import os
import shutil

# Default values
outputname = 'image.c'
compress = False
extflash = False
# Get real path of tool (script, converter and header, include, footer files)
# also escape string
toolpath = os.path.dirname(os.path.realpath(__file__)).replace('\\', '\\\\')
params = []
if len(sys.argv) > 2:
    outputname = sys.argv[1]
    for i in range(2, len(sys.argv)):
        if sys.argv[i] == '-c':
            compress = True
        if sys.argv[i] == '-extflash':
            extflash = True
        else:
            params.append(sys.argv[i])
else:
    print(
        'Concatenate.py outputfile.c target-device target-colour-depth [-extflash] [-c] [-b] [-f] [-r]')
    print('For more details see bm2rgbi converter parameters.')
    sys.exit()

# Set argument
if compress == True:
    if extflash == True:
        imageformat = 'IMAGE_LZGEXT'
    else:
        imageformat = 'IMAGE_LZG'
else:
    if extflash == True:
        imageformat = 'IMAGE_RAWEXT'
    else:
        imageformat = 'IMAGE_RAW'

# Collect image files
print('Running conversions in the background. Please wait...')
print('Starting conversion tasks...')
files = glob.glob('*.png')
files.extend(glob.glob('*.bmp'))
proc = []
# Run conversion on all image files in separate subprocesses (fastens up conversion on multiprocessor computers)
for fname in files:
    cmd = [toolpath + r'\bm2rgbi', fname, fname + '.out']
    cmd.extend(params)
    proc.append([subprocess.Popen(cmd, stdout=subprocess.PIPE), fname, 0, 0])

# Wait until all subprocesses are done
h = re.compile(r'Height:\s*(\d+)')
w = re.compile(r'Width:\s*(\d+)')
print('Waiting for tasks to finish...')
for p in proc:
    print
    print('Conversion result for ' + p[1])
    width = 0
    height = 0
    if p[0].returncode == None:
        p[0].wait()
    for line in p[0].stdout:
        rline = line.decode('utf-8')
        # Try to get width and height data from converter output
        m = h.match(rline)
        if m != None:
            height = m.group(1)
        m = w.match(rline)
        if m != None:
            width = m.group(1)
        print(" ", rline, end="")
    # Store width and height data
    p[2] = width
    p[3] = height

# Generate the output file
print('Generating ' + outputname + ' file.')
headername = outputname.replace('.c', '.h')

outputnamedef = headername.upper().replace('.', '_')
filenames = glob.glob("*.c")
images = []
maxnamelen = 5
totaldata = 0

if extflash == True:
    dump = open(outputname.replace('.c', '') + '.bin', 'wb')

with open(outputname, 'w') as outfile, open(headername, 'w') as headerfile:
    # Add header file first
    with open(toolpath + r'\concatenate_header.txt') as infile:
        print('Adding header.')
        for line in infile:
            outfile.write(line)
            headerfile.write(line)

    # Add warning not to edit the file manually
    msg = '\n\n/* THIS IS A GENERATED FILE, DO NOT EDIT IT!\n * See end of file for the included image list.\n */\n\n'
    outfile.write(msg)
    headerfile.write(msg)

    # Add ifndef to make sure file is only included once
    headerfile.write('#ifndef ' + outputnamedef + '\n')
    headerfile.write('#define ' + outputnamedef + '\n\n')

    # Add include file
    with open(toolpath + r'\concatenate_include.txt') as infile:
        print('Adding include.')
        for line in infile:
            headerfile.write(line)
        outfile.write('#include "' + headername + '"\n')

    # Regular expressions to match variables
    h = re.compile(r'unsigned char (?:(\w+)_out.*)?(?:(\w+).*)?')
    f = re.compile(r'unsigned int \w+ = (\d+);')
    for fname in filenames:
        fnameorig = fname.replace('.out.c', '')
        if fnameorig not in files:
            continue
        name = ''
        with open(fname) as infile:
            for line in infile:
                # If first variable (data struct) is matched
                m = h.match(line)
                if m != None:
                    # Grab its name
                    if m.group(1) != None:
                        name = m.group(1)
                    else:
                        name = m.group(2)
                    print('Adding image: ' + name)
                    # Add comment line for image
                    namelen = len(name) + 6
                    line = '\n' + '/*'.ljust(36 - namelen // 2, '*') + ' IMG_' + name.upper() + ' ' + '*/'.rjust(37 - namelen // 2 - namelen % 2, '*') + '\n\n'
                    outfile.write(line)
                    # New variable declaration for struct
                    # USED adds attribute used so the data stays fixed in the flash memory even if some of them are not used (less flash wearing during development)
                    # IMAGE adds a section attribute that has a fixed area in flash
                    line = 'const uint8_t ' + name + '[] IMAGE = {\n'
                    #line2 = 'extern uint8_t ' + name + '[];\n'
                    # headerfile.write(line2)
                # If the second variable is matched
                m = f.match(line)
                if m != None:
                    # Make sure its name is the same as the data struct with a _len postfix.
                    #line = 'USED static unsigned int ' + name + "_len = " + m.group(1) + ';\n\n'
                    line = ''
                    maxnamelen = max(len(name), maxnamelen)
                    # Get height and width data from process list
                    p = [x for x in proc if x[1] == fnameorig][0]
                    images.append([name, m.group(1), p[2], p[3]])
                    outfile.write(
                        '\nUSED const ImageDesc_t IMG_' + name.upper() + ' = {\n')
                    headerfile.write(
                        '\nextern const ImageDesc_t IMG_' + name.upper() + ';')
                    if extflash == False:
                        addr = name
                    else:
                        addr = '(void*)(FLASH_EXTIMAGE_OFFSET + ' + \
                            hex(totaldata) + ')'
                    outfile.write('  .width = ' + p[2] + ',\n  .height = ' + p[3] + ',\n  .image = ' +
                                  addr + ',\n  .len = ' + m.group(1) + ',\n  .format = ' + imageformat + '\n};\n')
                    totaldata += int(m.group(1))
                if extflash == False:
                    outfile.write(line)

        if extflash == True:
            shutil.copyfileobj(open(fname.replace('.c', ''), 'rb'), dump)

        # Remove temporary files
        os.remove(fname)
        os.remove(fname.replace('.c', ''))

    # Add export description to end of file
    numlen = len(str(totaldata))
    outfile.write('\n/* Image list\n')
    headerfile.write('\n\n/* Image list\n')
    for image in images:
        line = ' * IMG_' + image[0].upper().ljust(maxnamelen) + str(image[2]).rjust(
            5) + ' x' + str(image[3]).rjust(5) + ' : ' + str(image[1]).rjust(numlen) + ' bytes\n'
        outfile.write(line)
        headerfile.write(line)
    line = ' * ' + ''.ljust(maxnamelen + 30, '-') + '\n * ' + 'Total'.ljust(
        maxnamelen + 16) + ' : ' + str(totaldata).rjust(numlen) + ' bytes\n */\n'
    outfile.write(line)
    headerfile.write(line)

    # Add endif
    headerfile.write('\n#endif /* ' + outputnamedef + ' */\n')

    # Add a footer file at the end
    with open(toolpath + r'\concatenate_footer.txt') as infile:
        print('Adding footer.')
        for line in infile:
            outfile.write(line)

if extflash == True:
    dump.close()

print('Total image data: ' + str(totaldata) + ' bytes.')
print('Done.')
