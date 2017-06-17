del vk_shader.vert.spv
del vk_shader.frag.spv

glslangvalidator -V vk_shader.vert -o vk_shader.vert.spv
glslangvalidator -V vk_shader.frag -o vk_shader.frag.spv
