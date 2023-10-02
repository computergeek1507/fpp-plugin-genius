#!/bin/sh

echo "Running fpp-plugin-controller PreStart Script"

BASEDIR=$(dirname $0)
cd $BASEDIR
cd ..
make
