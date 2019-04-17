#!/bin/sh

CFLAGS=`pkg-config --cflags SoapySDR`
LDADD="`pkg-config --libs SoapySDR` -lliquid"

gcc $CFLAGS -c radio.c
gcc $CFLAGS -c transmit.c

gcc -o transmit radio.o transmit.o $LDADD
