#Introduction

cpprt is a C++ framework with a high level of abstraction. It exposes a high level public API exposing some runtime utils features and provide facilities to extends features  to support a specific implementation. 
cpprt depends only on the STL framework and is C++98 compliant.

#Build:

1. _using old build style % defined makefile_
make [OUTPUT_ROOT=<current dir by default>] [MODE=<release/debug>] [STATIC=<yes/no>]
successful generation will build a library in OUTPUT_ROOT/mode/CppRT.[a/so]

2. _using cmake method_
the default install directory is "/usr"

-If you want to install to another destination then use the CMAKE_INSTALL_PREFIX option to set the target dir. 

-If you whant to enable doxygen documenetation build then set CMAKE_INSTALL_PREFIX to ON

#Example : 
_build and install to mydir target directory by enabling doxygen build

>from root source dir_:

$mkdir buid

$cd build

$cmake .. -DCMAKE_INSTALL_PREFIX=/home/user/mydir  -DBUILD_DOCUMENTATION=ON

$make && make Doc && make install
