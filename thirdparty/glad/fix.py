#!/usr/bin/env python

import os

# Remove the duplicate definition of _glad_GL_loader_handle.
with open("gl.c", "r") as file:
    lines = file.readlines()
with open("gl.c", "w") as file:
    found_first = False
    for line in lines:
        if "static void* _glad_GL_loader_handle = NULL;" in line:
            if found_first:
                continue
            found_first = True
        file.write(line)