#!/bin/sh

chmod +x ./autogen.sh
./autogen.sh

rm -rf build

mkdir build
mkdir -f inst

cd build && ../configure --prefix `readlink -f ../inst` && make && make install

