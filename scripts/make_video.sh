#!/bin/bash
# ffmpeg expects numeric sequence; convert day_000.ppm ... to day_%d.ppm by removing leading zeros in name if needed
# We'll use pattern day_%03d.ppm directly
ffmpeg -y -framerate 6 -i output_merge/day_%03d.ppm -c:v libx264 -pix_fmt yuv420p video/brote.mp4
echo "Video saved to video/brote.mp4"