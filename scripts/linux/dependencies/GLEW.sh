#!/bin/bash


git submodule update --init third_party/glew
cd third_party/glew
cd auto && make && cd .. && make && sudo make install
echo "/usr/lib64" > usr-lib64.conf && sudo mv usr-lib64.conf /etc/ld.so.conf.d && sudo ldconfig

