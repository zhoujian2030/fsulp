#!/bin/sh

echo "Start dpe ..."
pkill -9 dpe
chmod +x dpe
./dpe -c dpe.conf & 

