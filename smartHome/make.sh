#! bin/bash

PWD=$(pwd)
echo $PWD
path="$PWD/driver/src"

cd $path/beep/
make
make install


cd $path/fan/
make
make install

cd $path/led
make
make install

cd $path/m74hc595
make
make install

cd $path/motor
make
make install

cd $path/si7006
make
make install
