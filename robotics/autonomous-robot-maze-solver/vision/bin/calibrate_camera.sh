#!/bin/bash

# Run the interactive calibrater
# Print out the calibration image in this folder
# Place constants into the camera_constatns.h file
# see https://docs.opencv.org/master/d7/d21/tutorial_interactive_calibration.html
../opencv/build/installation/OpenCV-3.4.6/bin/opencv_interactive-calibration -t=charuco -pf=mazeConfig.xml
#../opencv/build/installation/OpenCV-3.4.6/bin/opencv_interactive-calibration -t=chessboard -pf=mazeConfig.xml -sz=345 -w=9 -h=9
