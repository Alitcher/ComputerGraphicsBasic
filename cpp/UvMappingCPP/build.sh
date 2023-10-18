#!/bin/bash

# Compile individual source files into object files
g++ -c src/main.cpp -Iinclude -o obj/src/main.o
g++ -c src/shader_util.cpp -Iinclude -o obj/src/shader_util.o
g++ -c src/texture_util.cpp -Iinclude -o obj/src/texture_util.o

# Link object files to produce the final executable
g++ obj/src/*.o -Llib -lglfw -lGLEW -lIL -lILU -lILUT -lGL -lX11 -lpthread -lXrandr -lXi -o bin/uvMapping

echo "Build completed!"
