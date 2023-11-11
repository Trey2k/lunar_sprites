#!/usr/bin/env python

import os
import re

pattern = re.compile(".*glad_[^GLES2][a-zA-Z0-9_]+ = NULL;\n")

gl_methods = []


with open("gl.c", "r") as file:
    lines = file.readlines()

for line in lines:
        if pattern.match(line):
            gl_methods.append(line.split(" ")[1])

with open("gles2.c", "r") as file:
    lines = file.readlines()

print("Missing methods:") 
for method in gl_methods:
        found = False
        for line in lines:
            if method in line:
                found = True
                break
        
        if not found:
            print("\t" + method)

