
# Build using MSVC project or CMake

# Run tests
cd bin
GL_vs_VK.exe -t (1|2|3) -api (gl|vk) (-m)
```


### Linux

1. Clone or download this repository
2. Initialize GIT submodules
3. (Optional) Install system dependencies
4. Install project's third-party dependencies
5. Build
6. Run some tests

```
# Initialize repository
git clone https://github.com/RippeR37/GL_vs_VK.git`
cd GL_vs_VK
git submodule update --init

# Install dependencies
sudo apt-get install freeglut3-dev libvulkan1  # OpenGL/Vulkan deps
./scripts/linux/install_dependencies.sh

# Build
mkdir build && cd build
cmake ..
make -j 4

# Run tests
cd ../bin/
./GL_vs_VK -t (1|2|3) -api (gl|vk) (-m)
```

## Runtime arguments

`-t` - specifies test number

`-api` - specifies used api (`gl` or `vk`)

`-m` - (optional) specifies to run multithreaded version (not all tests available!)


## Author

I'm the only author of this repository and due to it's nature, for now I'm can't approve any contributions. If you have any notes or issues, please raise them and make sure to include your hardware, software and driver version (link to http://vulkan.gpuinfo.org entry would be nice).
