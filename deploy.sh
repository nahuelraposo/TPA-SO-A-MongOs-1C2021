#!/bin/bash

RULE=""
PROJECT="tp-2021-1c-LaMitad-1/"
UTILS="tp-2021-1c-LaMitad-1/utils/bin"

cd ..
echo "\n\nInstalando so-commons-library...\n\n"
git clone "https://github.com/sisoputnfrba/so-commons-library.git"
cd so-commons-library
sudo make install
cd ..

echo "\n\nInstalando ncurses...\n\n"
sudo apt-get install libncurses5-dev

echo "\n\nInstalando so-nivel-gui-library..\n\n"
git clone "https://github.com/sisoputnfrba/so-nivel-gui-library/"
cd so-nivel-gui-library
sudo make install
cd ..

cd
echo "\n\nDescargando pruebas a-mongo...\n\n"
git clone https://github.com/sisoputnfrba/a-mongos-pruebas.git

cd $PROJECT
make 

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$UTILS
export LD_LIBRARY_PATH

cd
cd $UTILS
sudo cp libutils.a /usr/lib

echo "\n\nDeploy hecho\n\n"
