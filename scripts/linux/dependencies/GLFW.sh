#!/bin/bash


sudo apt-get install xorg-dev libglu1-mesa-dev
git submodule update --init third_party/glfw
cd third_party/glfw
cmake -DBUILD_SHARED_LIBS=ON && sudo make install

