@echo off
if not exist "C:\Users\tutan\Downloads\epidemia_sir\video" mkdir "C:\Users\tutan\Downloads\epidemia_sir\video"
ffmpeg -y -framerate 6 -i "C:\Users\tutan\Downloads\epidemia_sir\output_merge\day_%%03d.ppm" -c:v libx264 -pix_fmt yuv420p "C:\Users\tutan\Downloads\epidemia_sir\video\brote.mp4"
echo Video saved to C:\Users\tutan\Downloads\epidemia_sir\video\brote.mp4
pause
