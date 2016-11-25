#mjpg_streamer -i input_uvc.so -d /dev/video0 -r 320x240 -f 25 -o output_http.so -p 8080 -w /www/webcam
mjpg_streamer -i input_uvc.so -d /dev/video0 -r 640x480 -f 25 -o output_http.so -p 8080 -w /www/webcam
