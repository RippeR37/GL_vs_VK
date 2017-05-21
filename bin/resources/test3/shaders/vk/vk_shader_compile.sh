#!/bin/bash

rm render.vert.spv
rm render.frag.spv
rm shadowmap.vert.spv
rm shadowmap.frag.spv

glslangValidator -V render.vert -o render.vert.spv
glslangValidator -V render.frag -o render.frag.spv
glslangValidator -V shadowmap.vert -o shadowmap.vert.spv
glslangValidator -V shadowmap.frag -o shadowmap.frag.spv
