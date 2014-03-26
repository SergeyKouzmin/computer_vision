#!/bin/bash

g++ -c -pipe -O2 -Wall -W -fPIE -I../background_separation -I. -o main.o ../background_separation/main.cpp

g++ -o background_separation main.o   -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc 