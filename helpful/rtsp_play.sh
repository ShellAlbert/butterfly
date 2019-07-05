#!/bin/bash

#gst-launch-1.0  playbin uri="rtsp://192.168.137.10:554/user=admin&password=&channel=1&stream=1.sdp"
#gst-launch-1.0  playbin uri="rtsp://192.168.137.10:554/user=admin&password=&channel=1&stream=0.sdp"
gst-launch-1.0  rtspsrc latency=0 location="rtsp://192.168.137.12:554/user=admin&password=&channel=1&stream=0.sdp" ! rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,width=1920,height=1080,format=BGRx ! videoconvert ! ximagesink sync=false
