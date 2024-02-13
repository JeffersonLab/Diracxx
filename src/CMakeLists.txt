if (Python_VERSION_MAJOR EQUAL 2)
    find_package(PythonLibs 2.7 REQUIRED)
    if (NOT DEFINED PYTHON_WITH_VERSION)
        set(PYTHON_WITH_VERSION "python")
    endif (NOT DEFINED PYTHON_WITH_VERSION)
elseif (Python_VERSION_MAJOR EQUAL 3)
    find_package(PythonLibs 3.6 REQUIRED)
    if (NOT DEFINED PYTHON_WITH_VERSION)
        if (Python_VERSION_MAJOR EQUAL 3 AND Python_VERSION_MINOR EQUAL 9)
            set(PYTHON_WITH_VERSION "python39")
        elseif (Python_VERSION_MAJOR EQUAL 3 AND Python_VERSION_MINOR EQUAL 6)
            set(PYTHON_WITH_VERSION "python3")
        else (Python_VERSION_MAJOR EQUAL 3 AND Python_VERSION_MINOR EQUAL 9)
            message("PYTHON_WITH_VERSION not defined, defaulting to python3")
            set(PYTHON_WITH_VERSION python3)
        endif (Python_VERSION_MAJOR EQUAL 3 AND Python_VERSION_MINOR EQUAL 9)
    endif (NOT DEFINED PYTHON_WITH_VERSION)
endif (Python_VERSION_MAJOR EQUAL 2)

# In case you need to check what python environment variables are available
# here, uncomment the code below and repeat the cmake command.
#
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

find_package(Boost 1.53 REQUIRED COMPONENTS system ${PYTHON_WITH_VERSION})

list(APPEND OBJECT_SOURCES
            TCrossSection.cxx
            TCrossSection_v1.cxx
            TDiracMatrix.cxx
            TDiracSpinor.cxx
            TFourVectorComplex.cxx
            TFourVectorReal.cxx
            TLepton.cxx
            TLorentzBoost.cxx
            TLorentzTransform.cxx
            TPauliMatrix.cxx
            TPauliSpinor.cxx
            TPhoton.cxx
            TThreeRotation.cxx
            TThreeVectorComplex.cxx
            TThreeVectorReal.cxx
           )
foreach(SRC IN LISTS OBJECT_SOURCES)
    string(REPLACE .cxx Dict.cxx DICT ${SRC})
    string(REPLACE .cxx .h HDR ${SRC})
    string(REPLACE .cxx LinkDef.h LINKDEF ${SRC})
    string(REPLACE .cxx Dict_rdict.pcm PCM ${SRC})
    list(APPEND DICT_SOURCES ${DICT})
    list(APPEND PCM_FILES ${CMAKE_CURRENT_BINARY_DIR}/${PCM})
    add_custom_command(OUTPUT ${DICT}
                       COMMAND rootcint -f ${DICT} -c ${CMAKE_SOURCE_DIR}/include/${HDR} ${CMAKE_SOURCE_DIR}/include/${LINKDEF} 2>/dev/null
                       DEPENDS ${SRC}
                      )
endforeach()

add_library(Dirac SHARED
            ${OBJECT_SOURCES}
            ${DICT_SOURCES}
            python_bindings.cxx
           )

target_link_libraries(Dirac ${Boost_LIBRARIES} ${PythonLibs_LIBRARIES})

set_target_properties(Dirac PROPERTIES VERSION ${PROJECT_VERSION}
                                       SOVERSION ${PROJECT_VERSION_MAJOR})

target_include_directories(Dirac PUBLIC
                           "${PROJECT_SOURCE_DIR}"
                           "${PROJECT_SOURCE_DIR}/include"
                           "${PROJECT_SOURCE_DIR}/rootmacros"
                           "${CMAKE_SOURCE_DIR}/src"
                           ${EXTRA_INCLUDES}
                          )

execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CFLAGS} -I ${Python_INCLUDE_DIRS}")

install(TARGETS Dirac
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
       )
install(FILES ${PCM_FILES} DESTINATION lib)
