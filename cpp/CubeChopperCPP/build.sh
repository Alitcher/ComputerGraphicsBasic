#!/bin/bash
g++ -o CubeChopperCPP Camera.cpp main.cpp shader_util.cpp -I./include -L/lib -lglfw -lGLEW -lGL
./CubeChopperCPP
