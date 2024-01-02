cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Diracxx VERSION 1.1 DESCRIPTION "Feynman diagram calculator library implementing Dirac algebra")
configure_file(DiracxxConfig.h.in DiracxxConfig.h)

# specify the C++ standard
if(CMAKE_CXX17_STANDARD_COMPILE_OPTION STREQUAL "-std=c++17")
  message("compile c++ code with c++17 support")
  set(CMAKE_CXX_STANDARD 17)
elseif(CMAKE_CXX14_STANDARD_COMPILE_OPTION STREQUAL "-std=c++14")
  message("compile c++ code with c++14 support")
  set(CMAKE_CXX_STANDARD 14)
elseif(CMAKE_CXX11_STANDARD_COMPILE_OPTION STREQUAL "-std=c++11")
  message("compile c++ code with c++11 support")
  set(CMAKE_CXX_STANDARD 11)
else(CMAKE_CXX17_STANDARD_COMPILE_OPTION STREQUAL "-std=c++17")
  message("compile c++ code with compiler default std")
endif(CMAKE_CXX17_STANDARD_COMPILE_OPTION STREQUAL "-std=c++17")
set(CMAKE_CXX_STANDARD_REQUIRED True)

# require python to be installed
if(PYTHON_GE_3)
    execute_process(COMMAND bash "-c" "python3 --version 2>&1"
                    COMMAND awk -F[.\ ] "/Python /{print $2}"
                    OUTPUT_VARIABLE PYTHON_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
else(DEFINED PYTHON_GE_3 and PYTHON_GE_3)
    execute_process(COMMAND bash "-c" "python --version 2>&1"
                    COMMAND awk -F[.\ ] "/Python /{print $2}"
                    OUTPUT_VARIABLE PYTHON_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(PYTHON_GE_3)

if(PYTHON_VERSION EQUAL 3)
  find_package(Python3 COMPONENTS Interpreter Development)
  set(PYTHON_VERSION_MAJOR 3)
  set(PYTHON_GE_3 True)
elseif(PYTHON_VERSION EQUAL 2)
  find_package(Python2 COMPONENTS Interpreter Development)
  set(PYTHON_VERSION_MAJOR 2)
  set(PYTHON_GE_3 False)
endif(PYTHON_VERSION EQUAL 3)

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
