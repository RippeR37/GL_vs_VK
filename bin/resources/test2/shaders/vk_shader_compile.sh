#!/bin/bash

rm vk_shader.vert.spv
rm vk_shader.frag.spv

glslangValidator -V vk_shader.vert -o vk_shader.vert.spv
glslangValidator -V vk_shader.frag -o vk_shader.frag.spv
