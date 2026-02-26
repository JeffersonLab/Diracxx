if(FORCE_PYTHON2)
    set(BOOST_PYTHON_COMPONENT "python27")
else()
    # Using Python3_VERSION_MAJOR/MINOR from your top-level file
    set(BOOST_PYTHON_COMPONENT "python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
endif()

find_package(Boost 1.53 REQUIRED COMPONENTS system ${BOOST_PYTHON_COMPONENT})


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

target_link_libraries(Dirac PUBLIC ${Boost_LIBRARIES} ${PYTHON_TARGET})

set_target_properties(Dirac PROPERTIES VERSION ${PROJECT_VERSION}
                                       SOVERSION ${PROJECT_VERSION_MAJOR})

target_include_directories(Dirac PUBLIC
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/rootmacros>
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
                           $<INSTALL_INTERFACE:include>
                          )

execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CFLAGS} -I ${Python_INCLUDE_DIRS}")

install(TARGETS Dirac
        EXPORT DiracxxTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
       )
install(FILES ${PCM_FILES} DESTINATION lib64)
