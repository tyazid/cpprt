#Introduction

cpprt is a C++ framework with a high level of abstraction. It exposes a high level public API exposing some runtime utils features and provide facilities to extends features  to support a specific implementation. 
cpprt depends only on the STL framework and is C++98 compliant.

#Build:

1. using old build style % defined makefile
make [OUTPUT_ROOT=<current dir by default>] [MODE=<release/debug>] [STATIC=<yes/no>]
successful generation will build a library in OUTPUT_ROOT/mode/CppRT.[a/so]

2. using cmake method
the default install directory is "/usr"

-If you want to install to another destination then use the CMAKE_INSTALL_PREFIX option to set the target dir. 

-If you want to enable doxygen documenetation build then set BUILD_DOCUMENTATION to ON

#Example : 
build and install to mydir target directory by enabling doxygen build

>from root source dir:

$mkdir buid

$cd build

$cmake .. -DCMAKE_INSTALL_PREFIX=/home/user/mydir  -DBUILD_DOCUMENTATION=ON

$make && make Doc && make install
