# DTV Git repository:
http://USER@stash.fr.nds.com/scm/~tyazid/dtv.git
# DTV/Lib:
is a C++ framework with a high level of abstraction. It exposes a high level public API dedicated to Digital TV features and provide facilities to extends features namespaces to support a specific content type dtv implementation such as content-referencing, system-information, media framework. 
DtvLib depends only on the STL framework and is C++98 compliant.

##  Build:
1.
using old build style % defined makefile
make [OUTPUT_ROOT=<current dir by default>] [MODE=<release/debug>] [STATIC=<yes/no>]
successful generation will build a library in OUTPUT_ROOT/mode/CppRT.[a/so]

2.
using cmake method,
the default install directory is "/usr"
-If you want to install to another destination the use the CMAKE_INSTALL_PREFIX option to set the target dir. 
-If you whant to enable doxygen documenetation build the set CMAKE_INSTALL_PREFIX to ON
example : 
build and install to mydir install target by enabling doxygen build

from root source dir:
$mkdir buid
$cd build
$cmake .. -DCMAKE_INSTALL_PREFIX=/home/user/mydir  -DBUILD_DOCUMENTATION=ON
$make && make Doc && make install

 
    