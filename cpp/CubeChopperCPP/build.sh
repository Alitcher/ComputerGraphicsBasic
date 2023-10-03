#!/bin/bash
g++ -o CubeChopperCPP main.cpp shader_util.cpp -I./include -L/lib -lglfw -lGLEW -lGL
./CubeChopperCPP
