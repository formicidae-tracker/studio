#!/bin/sh

set -ex

git clone https://github.com/formicidae-tracker/hermes.git
cd hermes
git checkout v0.2.2
mkdir build
cd build
cmake ../
make
sudo make install
