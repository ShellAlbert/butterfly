#!/bin/bash
amixer -c tegrasndt186ref sset "ADMAIF1 Mux" "I2S1"
arecord -D hw:tegrasndt186ref,0 -r 32000 -f S32_LE -t raw -c 2 -d 100 cap.pcm

