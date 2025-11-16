# keylogger
Keylogger deaemon on linux system.

# Requirments
You need to install libevdev library on your linux system and i think you need run evtest to find if the keyboard driver is mounted on /dev/input/event0 because on my computer its working but on the other couldn't work. Im just lazy to write function to automaticly find right driver.

# How to run?
run this command in terminal gcc key.c -o key $(pkg-config --cflags --libs libevdev)

# Why that so crappy ? 
I just wanted to make it working and after that i got bored
