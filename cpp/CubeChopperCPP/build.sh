#!/bin/bash
g++ -o ShaderChopperCPP main.cpp shader_util.cpp -I./include -L/lib -lglfw -lGLEW -lGL
./ShaderChopperCPP
