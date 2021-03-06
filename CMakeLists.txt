cmake_minimum_required(VERSION "3.19.3")
project("Analysis")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#Use correct GSL library for each system
if (WIN32)
    set(LIB_ROOT_DIR lib/Windows)
    set(GSL_ROOT_DIR ${LIB_ROOT_DIR})
    set(GSL_LIBRARY_SUFFX dll)
else ()
    set(LIB_ROOT_DIR lib/MacOS)
    set(GSL_ROOT_DIR ${LIB_ROOT_DIR})
    set(GSL_LIBRARY_SUFFIX a)
endif()

set(SHARED_LIB_PREFIX ${LIB_ROOT_DIR}/bin)

#Find and build GSL library
find_package(GSL REQUIRED)

#Set Sources
set(SOURCES src/main.cc src/simulator.cc src/simulation.cc)
set(DEBUG_TARGET ${PROJECT_NAME}_debug)
#Build Executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE GSL::gsl GSL::gslcblas)
target_include_directories(${PROJECT_NAME} PRIVATE include/)
target_compile_definitions(${PROJECT_NAME} PRIVATE ANALYSIS_PROJECT_INSTALLED)

#Build debug exeuctable 
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(${DEBUG_TARGET} ${SOURCES})
    target_include_directories(${DEBUG_TARGET} PRIVATE include/)
    target_link_libraries(${DEBUG_TARGET} PRIVATE GSL::gsl GSL::gslcblas)
endif()

#Set optimization 
#The MSVC compiler takes different compiler flags.
#Therefore we must check to make sure it is not the compiler being used
#if we are passing GCC optimization flags. 
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Ofast")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Werror")
    target_compile_options(${PROJECT_NAME} PRIVATE "-pedantic")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE "/O2")
endif()

#Set install folder properties
set(CMAKE_INSTALL_PREFIX "../Analysis")

#Make install folder
#Install libraries
if (WIN32)
    file(GLOB bin_files ${SHARED_LIB_PREFIX}/*.dll)
    foreach(file ${BIN_FILES})
        install(FILES file DESTINATION bin)
    endforeach()
endif()
#Instal models
file(GLOB model_files "models/*.txt")
foreach(file ${model_files}) 
    install(FILES ${file} DESTINATION model)
endforeach()
#Install Target
install(TARGETS ${PROJECT_NAME} DESTINATION "bin")
