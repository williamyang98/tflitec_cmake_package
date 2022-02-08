# Introduction
This is a config.cmake for the lightweight tensorflowlite_c api. 

Build targets are:
- host=x64 target=x64
- host=x86 target=x86

# Build environment
Built using 2019 Visual Studio
msbuild = 16.10.0+4242f381a
cl = 19.29.30037 (for x86 and x64)
# Instructions
1. Follow steps here: https://www.tensorflow.org/lite/guide/build_cmake
2. Fix a few msvc incompatibilities with existing tflite (c++ api) code
  - M_PI not defined in random/
  - Need to cast (const char*) to (const wchar_t*) (Is this okay???)
  - Need to change CXX standard of tensorflowlite cmake target to CXX_20 for tag based initialiser of structs

# Example code
Sample program is shown in examples/
