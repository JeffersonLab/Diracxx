cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Diracxx VERSION 1.1 DESCRIPTION "Feynman diagram calculator library implementing Dirac algebra")
configure_file(DiracxxConfig.h.in DiracxxConfig.h)

# require python to be installed
if(FORCE_PYTHON2)
  message("choosing python2 over python3...")
  find_package(Python2 REQUIRED COMPONENTS Interpreter Development)
  set(Python_INCLUDE_DIRS ${Python2_INCLUDE_DIRS})
  set(PYTHON_TARGET Python2::Python)
else(FORCE_PYTHON2)
  find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
  set(Python_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
  set(PYTHON_TARGET Python3::Python)
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
install(CODE "execute_process(COMMAND ln -s ../lib64/libDirac.so ${CMAKE_INSTALL_PREFIX}/python/libDirac.so)")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(PATH_VARS_DIR "lib/cmake/Diracxx")
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/DiracxxConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DiracxxConfig.cmake"
    INSTALL_DESTINATION ${PATH_VARS_DIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DiracxxConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/DiracxxConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DiracxxConfigVersion.cmake"
    DESTINATION ${PATH_VARS_DIR}
)
install(EXPORT DiracxxTargets
    FILE DiracxxTargets.cmake
    NAMESPACE Diracxx::
    DESTINATION ${PATH_VARS_DIR}
)
