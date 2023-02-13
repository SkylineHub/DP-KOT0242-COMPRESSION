cmake_minimum_required(VERSION 3.21)
project(compresscontroller)

set(CMAKE_CXX_STANDARD 17)

add_executable(compresscontroller main.cpp Node.h Compress.h Bitlib.h BitBuffer.h Definitions.h ArrayUtils.h Arithmetic.h AdaptiveArithmetic.h Decompress.h Model.h DebugHelper.h DebugHelper.h)
