cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Diracxx VERSION 1.1 DESCRIPTION "Feynman diagram calculator library implementing Dirac algebra")
configure_file(DiracxxConfig.h.in DiracxxConfig.h)

# require python to be installed
if(FORCE_PYTHON2)
  message("choosing python2 over python3...")
  find_package(Python2 REQUIRED COMPONENTS Interpreter Development)
  set(Python_INCLUDE_DIRS ${Python2_INCLUDE_DIRS})
else(FORCE_PYTHON2)
  find_package(Python REQUIRED COMPONENTS Interpreter Development)
endif(FORCE_PYTHON2)

# build libDirac from sources
add_subdirectory(src)

# default installation area
execute_process(COMMAND uname -m OUTPUT_VARIABLE TARGET_ARCH
                                 OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ls ${CMAKE_SOURCE_DIR}/include 
                                 OUTPUT_VARIABLE INCLUDE_FILES
                                 OUTPUT_STRIP_TRAILING_WHITESPACE)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/${TARGET_ARCH} CACHE PATH "Default install directory is under the source tree" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include DESTINATION .)
install(FILES ${CMAKE_BINARY_DIR}/DiracxxConfig.h DESTINATION include)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/rootmacros DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/python DESTINATION .)

# add a symlink to libDirac.so in the python directory
install(CODE "execute_process(COMMAND ln -s ../lib/libDirac.so ${CMAKE_INSTALL_PREFIX}/python/libDirac.so)")
