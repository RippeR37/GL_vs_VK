#!/bin/bash


git submodule update --init third_party/glm
cd third_party/glm
cmake . && sudo make install

