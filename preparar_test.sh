#!/bin/bash

RULE=""
PROJECT="tp-2021-1c-LaMitad-1/"
DISCORDIADOR="discordiador"
MONGO="i-Mongo-Store"
RAM="Mi-RAM-HQ"
G_IP_RAM="10.108.176.3"
G_IP_DISC="10.108.176.4"
G_IP_MONGO="10.108.176.2"

echo "\n\n"
read -r -p "~~~Press enter to continue...~~~" 1 -n -s
echo "\n\n"

echo "\n\n\t\t~~~Estabilidad General~~~\n\n"

cd $DISCORDIADOR

if test -e discordiador.config; then
    rm discordiador.config
fi

echo "IP_MI_RAM_HQ=$G_IP_RAM
PUERTO_MI_RAM_HQ=5002
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_I_MONGO_STORE=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_ESCUCHA=5001
GRADO_MULTITAREA=2
ALGORITMO=RR
QUANTUM=1
DURACION_SABOTAJE=10
RETARDO_CICLO_CPU=1
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/discordiador/discordiador.log" >> discordiador.config

cd ..
cd $MONGO

if test -e i-Mongo-Store.config; then
    rm i-Mongo-Store.config
fi

echo "PUNTO_MONTAJE=/home/utnso/tp-2021-1c-LaMitad-1/polus
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_ESCUCHA=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/i-Mongo-Store/i-Mongo-Store.log
TIEMPO_SINCRONIZACION=12
POSICIONES_SABOTAJE=[1|1]
BLOCK_SIZE=256
BLOCKS=32768" >> i-Mongo-Store.config

cd ..
cd $RAM

if test -e Mi-RAM-HQ.config; then
    rm Mi-RAM-HQ.config
fi

echo "TAMANIO_MEMORIA=16384
ESQUEMA_MEMORIA=SEGMENTACION
TAMANIO_PAGINA=64
TAMANIO_SWAP=16384
ALGORITMO_REEMPLAZO=LRU
CRITERIO_SELECCION=FF
IP_RAM=$G_IP_RAM
PUERTO_ESCUCHA=5002
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/Mi-RAM-HQ/Mi-RAM-HQ.log
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_SWAP=/home/utnso/tp-2021-1c-LaMitad-1/swapFile.bin" >> Mi-RAM-HQ.config

cd ..
echo "\n\n\t\t"
read -r -p "~~~Press enter to continue...~~~" 1 -n -s
echo "\n\n"
echo "\n\n\t\t~~~Discordiador - CPU~~~\n"
echo "\t\t~~~Discordiador - E/S~~~\n\n"

cd $DISCORDIADOR

if test -e discordiador.config; then
    rm discordiador.config
fi

echo "IP_MI_RAM_HQ=$G_IP_RAM
PUERTO_MI_RAM_HQ=5002
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_I_MONGO_STORE=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_ESCUCHA=5001
GRADO_MULTITAREA=2
ALGORITMO=FIFO
QUANTUM=3
DURACION_SABOTAJE=10
RETARDO_CICLO_CPU=1
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/discordiador/discordiador.log" >> discordiador.config

cd ..
cd $MONGO

if test -e i-Mongo-Store.config; then
    rm i-Mongo-Store.config
fi

echo "PUNTO_MONTAJE=/home/utnso/tp-2021-1c-LaMitad-1/polus
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_ESCUCHA=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/i-Mongo-Store/i-Mongo-Store.log
TIEMPO_SINCRONIZACION=12
POSICIONES_SABOTAJE=[1|1]
BLOCK_SIZE=64
BLOCKS=16384" >> i-Mongo-Store.config

cd ..
cd $RAM

if test -e Mi-RAM-HQ.config; then
    rm Mi-RAM-HQ.config
fi

echo "TAMANIO_MEMORIA=8192
ESQUEMA_MEMORIA=SEGMENTACION
TAMANIO_PAGINA=64
TAMANIO_SWAP=16384
ALGORITMO_REEMPLAZO=LRU
CRITERIO_SELECCION=FF
IP_RAM=$G_IP_RAM
PUERTO_ESCUCHA=5002
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/Mi-RAM-HQ/Mi-RAM-HQ.log
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_SWAP=/home/utnso/tp-2021-1c-LaMitad-1/swapFile.bin" >> Mi-RAM-HQ.config

cd ..
echo "\n\n\t\t"
read -r -p "~~~Press enter to continue...~~~" 1 -n -s
echo "\n\n"

echo "\n\n\t\t~~~Memoria - Segmentación~~~\n\n"

cd $DISCORDIADOR

if test -e discordiador.config; then
    rm discordiador.config
fi

echo "IP_MI_RAM_HQ=$G_IP_RAM
PUERTO_MI_RAM_HQ=5002
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_I_MONGO_STORE=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_ESCUCHA=5001
GRADO_MULTITAREA=2
ALGORITMO=RR
QUANTUM=3
DURACION_SABOTAJE=10
RETARDO_CICLO_CPU=2
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/discordiador/discordiador.log" >> discordiador.config

cd ..
cd $MONGO

if test -e i-Mongo-Store.config; then
    rm i-Mongo-Store.config
fi

echo "PUNTO_MONTAJE=/home/utnso/tp-2021-1c-LaMitad-1/polus
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_ESCUCHA=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/i-Mongo-Store/i-Mongo-Store.log
TIEMPO_SINCRONIZACION=10
POSICIONES_SABOTAJE=[1|1]
BLOCK_SIZE=64
BLOCKS=16384" >> i-Mongo-Store.config

cd ..
cd $RAM

if test -e Mi-RAM-HQ.config; then
    rm Mi-RAM-HQ.config
fi

echo "TAMANIO_MEMORIA=256
ESQUEMA_MEMORIA=SEGMENTACION
TAMANIO_PAGINA=32
TAMANIO_SWAP=4096
ALGORITMO_REEMPLAZO=LRU
CRITERIO_SELECCION=FF
IP_RAM=$G_IP_RAM
PUERTO_ESCUCHA=5002
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/Mi-RAM-HQ/Mi-RAM-HQ.log
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_SWAP=/home/utnso/tp-2021-1c-LaMitad-1/swapFile.bin" >> Mi-RAM-HQ.config

cd ..
echo "\n\n\t\t"
read -r -p "~~~Press enter to continue...~~~" 1 -n -s
echo "\n\n"

echo "\n\n\t\t~~~Memoria - Paginación~~~\n\n"

cd $DISCORDIADOR

if test -e discordiador.config; then
    rm discordiador.config
fi

echo "IP_MI_RAM_HQ=$G_IP_RAM
PUERTO_MI_RAM_HQ=5002
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_I_MONGO_STORE=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_ESCUCHA=5001
GRADO_MULTITAREA=1
ALGORITMO=RR
QUANTUM=5
DURACION_SABOTAJE=10
RETARDO_CICLO_CPU=2
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/discordiador/discordiador.log" >> discordiador.config

cd ..
cd $MONGO

if test -e i-Mongo-Store.config; then
    rm i-Mongo-Store.config
fi

echo "PUNTO_MONTAJE=/home/utnso/tp-2021-1c-LaMitad-1/polus
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_ESCUCHA=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/i-Mongo-Store/i-Mongo-Store.log
TIEMPO_SINCRONIZACION=10
POSICIONES_SABOTAJE=[1|1]
BLOCK_SIZE=64
BLOCKS=16384" >> i-Mongo-Store.config

cd ..
cd $RAM

if test -e Mi-RAM-HQ.config; then
    rm Mi-RAM-HQ.config
fi

echo "TAMANIO_MEMORIA=128
ESQUEMA_MEMORIA=PAGINACION
TAMANIO_PAGINA=32
TAMANIO_SWAP=16384
ALGORITMO_REEMPLAZO=LRU
CRITERIO_SELECCION=FF
IP_RAM=$G_IP_RAM
PUERTO_ESCUCHA=5002
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/Mi-RAM-HQ/Mi-RAM-HQ.log
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_SWAP=/home/utnso/tp-2021-1c-LaMitad-1/swapFile.bin" >> Mi-RAM-HQ.config

cd ..
echo "\n\n\t\t"
read -r -p "~~~Press enter to continue...~~~" 1 -n -s
echo "\n\n"

echo "\n\n\t\t~~~File System~~~\n\n"

cd $DISCORDIADOR

if test -e discordiador.config; then
    rm discordiador.config
fi

echo "IP_MI_RAM_HQ=$G_IP_RAM
PUERTO_MI_RAM_HQ=5002
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_I_MONGO_STORE=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_ESCUCHA=5001
GRADO_MULTITAREA=3
ALGORITMO=FIFO
QUANTUM=1
DURACION_SABOTAJE=10
RETARDO_CICLO_CPU=1
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/discordiador/discordiador.log" >> discordiador.config

cd ..
cd $MONGO

if test -e i-Mongo-Store.config; then
    rm i-Mongo-Store.config
fi

echo "PUNTO_MONTAJE=/home/utnso/tp-2021-1c-LaMitad-1/polus
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_ESCUCHA=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/i-Mongo-Store/i-Mongo-Store.log
TIEMPO_SINCRONIZACION=5
POSICIONES_SABOTAJE=[1|1]
BLOCK_SIZE=32
BLOCKS=16384" >> i-Mongo-Store.config

cd ..
cd $RAM

if test -e Mi-RAM-HQ.config; then
    rm Mi-RAM-HQ.config
fi

echo "TAMANIO_MEMORIA=2048
ESQUEMA_MEMORIA=SEGMENTACION
TAMANIO_PAGINA=32
TAMANIO_SWAP=16384
ALGORITMO_REEMPLAZO=LRU
CRITERIO_SELECCION=FF
IP_RAM=$G_IP_RAM
PUERTO_ESCUCHA=5002
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/Mi-RAM-HQ/Mi-RAM-HQ.log
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_SWAP=/home/utnso/tp-2021-1c-LaMitad-1/swapFile.bin" >> Mi-RAM-HQ.config

cd ..
echo "\n\n\t\t"
read -r -p "~~~Press enter to continue...~~~" 1 -n -s
echo "\n\n"


echo "\n\n\t\t~~~File System - Sabotajes~~~\n\n"

cd $DISCORDIADOR

if test -e discordiador.config; then
    rm discordiador.config
fi

echo "IP_MI_RAM_HQ=$G_IP_RAM
PUERTO_MI_RAM_HQ=5002
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_I_MONGO_STORE=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_ESCUCHA=5001
GRADO_MULTITAREA=1
ALGORITMO=FIFO
QUANTUM=1
DURACION_SABOTAJE=10
RETARDO_CICLO_CPU=1
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/discordiador/discordiador.log" >> discordiador.config

cd ..
cd $MONGO

if test -e i-Mongo-Store.config; then
    rm i-Mongo-Store.config
fi

echo "PUNTO_MONTAJE=/home/utnso/tp-2021-1c-LaMitad-1/polus
IP_I_MONGO_STORE=$G_IP_MONGO
PUERTO_ESCUCHA=5003
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/i-Mongo-Store/i-Mongo-Store.log
TIEMPO_SINCRONIZACION=1
POSICIONES_SABOTAJE=[1|1,7|1,7|7,1|7,4|4]
BLOCK_SIZE=32
BLOCKS=16384" >> i-Mongo-Store.config

cd ..
cd $RAM

if test -e Mi-RAM-HQ.config; then
    rm Mi-RAM-HQ.config
fi

echo "TAMANIO_MEMORIA=2048
ESQUEMA_MEMORIA=SEGMENTACION
TAMANIO_PAGINA=32
TAMANIO_SWAP=16384
ALGORITMO_REEMPLAZO=LRU
CRITERIO_SELECCION=FF
IP_RAM=$G_IP_RAM
PUERTO_ESCUCHA=5002
PATH_ARCHIVO_LOG=/home/utnso/tp-2021-1c-LaMitad-1/Mi-RAM-HQ/Mi-RAM-HQ.log
IP_DISCORDIADOR=$G_IP_DISC
PUERTO_DISCORDIADOR=5001
PATH_SWAP=/home/utnso/tp-2021-1c-LaMitad-1/swapFile.bin" >> Mi-RAM-HQ.config

cd ..
echo "\n\n\t\t"
read -r -p "~~~Press enter to continue...~~~" 1 -n -s
echo "\n\n"


echo "\n\nDeploy hecho\n\n"
