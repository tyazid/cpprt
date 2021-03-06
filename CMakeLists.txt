CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
PROJECT(cpprt CXX)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
INCLUDE(doc)


SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR ${PREFIX}/lib)
SET(INCLUDEDIR ${PREFIX}/include)
SET(VERSION_MAJOR 1)
SET(VERSION "${VERSION_MAJOR}.0.0")
SET(PRJDESC "CppRT tools")
#SET(LIBDIR_INST ${PREFIX}/lib)
SET(C_PREFIX ${PREFIX})
SET(SRCS ${CMAKE_SOURCE_DIR}/src/cpprt.cpp) 
include_directories(${CppRt_SOURCE_DIR}/inc ${CppRt_SOURCE_DIR}/src)
link_directories(${CppRt_BINARY_DIR}/src)


SET(CMAKE_CXX_FLAGS_DEBUG "-fPIC  -O0  -Wall -c")
SET(CMAKE_CXX_FLAGS_RELEASE "{$CMAKE_C_FLAGS_DEBUG} -g -DDEBUG")
 
IF(DEFINED USE_SET_ERRNO_INTERNAL)
ADD_DEFINITIONS (-DSET_ERRNO_INTERNAL=1 )
ENDIF(DEFINED USE_SET_ERRNO_INTERNAL)


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/include")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${VERSION_MAJOR})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${VERSION})

SET(PC_PREFIX ${PREFIX})
SET(PC_VERSION ${VERSION})
SET(PC_DESC ${PRJDESC})
CONFIGURE_FILE(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.pc")

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${LIBDIR}/pkgconfig)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/inc/cpprt.h DESTINATION ${INCLUDEDIR})
INSTALL(FILES ${CMAKE_SOURCE_DIR}/inc/dbg.h DESTINATION ${INCLUDEDIR})


#Packaging
MESSAGE("CPACK_DEBIAN_PACKAGE_ARCHITECTURE = ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
SET(CPACK_GENERATOR "DEB;RPM")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "TYazid@gmail.com")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PRJDESC})
set(CPACK_PACKAGE_RELEASE 1)
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
#armhf

INCLUDE(CPack)
