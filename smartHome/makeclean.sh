#! bin/bash

PWD=$(pwd)
path="$PWD/driver/src"

cd $path/beep/
make clean


cd $path/fan/
make clean

cd $path/led
make clean

cd $path/m74hc595
make clean

cd /home/ubuntu/hqyj/smart_home/smartHome/driver/src/motor/
cd $path/motor
make
make clean

cd $path/si7006
make clean
