OpenGL vs Vulkan
================

This project is part of my master-thesis and aims to compare OpenGL and Vulkan API in terms of API-related overhead and performance user can gain (or lose) by using given API. Core idea resolves around creating pairs of almost-identical tests in each API and benchmark them.


### Disclaimer :warning:

Tests used in this project are fairly simple and doesn't involve any advanced techniques to avoid drivers overhead in OpenGL (commonly known as AZDO techniques). The reason for this is to be able to measure raw overhead of each API and their drivers instead of implementing something in most-efficient way possible. The tests were also chosen to replicate some real-life scenarios, so they don't use some available features that to replicate situations where they can't be used (e.g. I'm not using instancing becasue I want to simulate a scene with multile different objects to be rendered).


## Implemented tests


| Test \ API implementation | OpenGL | Multithreaded OpenGL | Vulkan | Multithreaded Vulkan |
| :---: | :---: | :---: | :---: | :---: |
| Test #1 | ✅ | ✅ | ✅ | ✅ |
| Test #2 | ✅ | ❌ | ✅ | ✅ |
| Test #3 | ✅ | ❌ | ✅ | ✅ |
| Test #4 | ✅ | ❌ | ✅ | ❌ |

Test #2 and #3 doesn't have multithreaded version with OpenGL API as they don't have easily scalable CPU-bound work that could benefit from dispatching to multiple threads.


### Test #1 - static scene

This test resolves around single static scene with variable number of rendered objects which quality can be chosen (each is a sphere with specific ammount of vertices).

Number of vertices, number of vertices and update work is customizable to give possibility to emulate different ammount of CPU and GPU work (this gives us an opportunity to test CPU-bound and GPU-bound scenarios).


### Test #2 - terrain with dynamic LoD

In this test, a terrain is rendered (as wireframe) with active dynamic level-of-detail changes. Each vertex is colored in fragment shader with regard to it's height. At startup, it loads 1024x1024 heightmap and pre-computes any necessary data. At runtime it traverses quad-tree structure and decides which elements should be rendered with current resolution, or if map's segment should be rendered with higher resoltuion and thus the node should be split.


### Test #3 - shadow mapping 

In this test we render a "checkboard" floor with differently-colored cubes and above that we render one high-res sphere and many cubes in different positions. We render it in two passes - depth pass from light PoV to acquire shadowmap and then real render pass which simply renderes scene shadowing necessary fragments.


### Test #4 - initialization

Test measures time from initialization start of OpenGL/Vulkan objects up to first draw (and present/swap buffer) call, synchronizes between CPU and GPU to make sure that everything is executed and measures elapsed time. This is non-practical test, but I wanted to know how both APIs behave. For Vulkan I've added support for pipeline cache.


## Results

This project was described and tested by **Michael Larabel** at [Phoronix.com](Phoronix.com).

Link to article with results: http://www.phoronix.com/scan.php?page=article&item=gl-vs-vk.

Results of my tests will be posted here at a later time.


## Building

### Requirements

* C++11 compiler
  * MSVC project provided for Windows
  * CMake project script provivded for Windows and Linux
* VulkanSDK might be needed on Windows (v1.0.46.0 or higher)
  * Vulkan-Hpp is one of dependencies
* Hardware and drivers supporting OpenGL and Vulkan
* OpenGL and Vulkan drivers


### Windows

1. Clone or download this repository
2. Initialize GIT submodules
3. (Optional) Install OpenGL/Vulkan system dependencies (drivers etc.)
4. Install project's third-party dependencies (script available in `scripts/` directory)
4.1. If you're using provided MSVC project, make sure that `third_party/glew-win/glew-win-src.zip` is extracted.
5. Build
6. Run some tests

```
git clone https://github.com/RippeR37/GL_vs_VK.git
cd GL_vs_VK
git submodule update --init

# Install third-party dependencies
cd scripts/windows/
install_msvc_dependencies.bat
cd ../..

# Build using MSVC project or CMake

# Run tests
cd bin
GL_vs_VK.exe -t (1|2|3) -api (gl|vk) (-m)
```


### Linux

1. Clone or download this repository
2. Initialize GIT submodules
3. Install dependencies
4. Build
5. Run some tests

```
# Initialize repository
git clone https://github.com/RippeR37/GL_vs_VK.git
cd GL_vs_VK
git submodule update --init

# Install dependencies
sudo apt-get install xorg-dev libgl1-mesa-dev libglu1-mesa-dev libvulkan-dev

# Build
mkdir build && cd build
cmake ..
make -j 4

# Run tests
cd ../bin/
./GL_vs_VK -t (1|2|3) -api (gl|vk) (-m)
```

## Runtime arguments

| Name | Argument type | Description |
| :--- | :---: | :---: |
| `-t` | integer | Specifies test number. |
| `-api` | string | Specifies used API. Valid options: `gl`, `vk`. |
| `-m` | - | Optional. Asks for multithreaded version of test (might not be available). |
| `-benchmark` | - | Optional. Enables benchmarking mode. |
| `-time` | float | Optional. Changes default time of test benchmarking. |

In benchmarking mode, test will end automatically in some time (default: 15 seconds, but can be changed with `-time` argument), after which statistics will be presented on screen.
Test 4 will always run in benchmark mode.


## Author

I'm the only author of this repository and due to it's nature, for now I can't approve any code contributions. If you have any notes or issues, please raise them and make sure to include your hardware, software and driver version (link to http://vulkan.gpuinfo.org entry would be nice).


## Contributions

* [@hadrien-psydk](https://github.com/hadrien-psydk) - testing & debugging on AMD GPU
