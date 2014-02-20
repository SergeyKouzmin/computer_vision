#!/bin/bash

#LD_LIBRARY_PATH="/usr/local/lib" ./blobtrack_sample fg=FG_1 bd=BD_Simple bt=MSFG btpp=Kalman bta=HistSS avi_name="./tree.avi" 
#LD_LIBRARY_PATH="/usr/local/lib" ./blobtrack_sample fg=FG_1 bd=BD_Simple bt=MSFG btpp=Kalman avi_name="./tree.avi"
LD_LIBRARY_PATH="/usr/local/lib" ./blobtrack_sample ./iq720p.mp4 fg=FG_1 bd=BD_Simple bt=MSFG

