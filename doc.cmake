option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
MESSAGE(" ----- IN DOC ... BUILD_DOCUMENTATION = ${BUILD_DOCUMENTATION}")
MESSAGE(" ----- IN DOC ... CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
file(COPY  ${CMAKE_SOURCE_DIR}/inc DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
SET(DOC_CMP "Doc")
if(BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()  
 ADD_CUSTOM_TARGET(${DOC_CMP} ${DOXYGEN} ${CMAKE_SOURCE_DIR}/RT.doxyfile ) 
  MESSAGE(" ----- IN DOC ... DOXYGEN_EXECUTABLE= ${DOXYGEN_EXECUTABLE}")
  
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DOC_CMP}  DESTINATION ${CMAKE_INSTALL_PREFIX})

endif()