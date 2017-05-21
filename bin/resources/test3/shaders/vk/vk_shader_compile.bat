del render.vert.spv
del render.frag.spv
del shadowmap.vert.spv
del shadowmap.frag.spv

glslangvalidator -V render.vert -o render.vert.spv
glslangvalidator -V render.frag -o render.frag.spv
glslangvalidator -V shadowmap.vert -o shadowmap.vert.spv
glslangvalidator -V shadowmap.frag -o shadowmap.frag.spv
