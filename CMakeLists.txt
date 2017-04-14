cmake_minimum_required(VERSION 2.6)
project(python-script-util)


#
# Build system variables
#

#Sets c++11 flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


#Workaround for bug on some ubuntu systems where CMake does not search for anything above Python 3.3
set(Python_ADDITIONAL_VERSIONS 3.4)

#
# Preprocessor macro's and variables
#

# Sets a preprocessor variable to enable os specific code to check the current build target
if(UNIX)
    add_definitions(-DTARGET_OS_UNIX_LIKE=1)
elseif(WIN32)
    add_definitions(-DTARGET_OS_WINDOWS=1)
endif()

#
# Importing Boost
#
find_package(Boost 1.54.0 REQUIRED python)

add_library(boost SHARED IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})

add_library(boost-python SHARED IMPORTED)
set_property(TARGET boost-python PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})

if(DEFINED PYTHON_SCRIPT_UTIL_BOOST_PYTHON_LIBRARY_OVERRIDE)
	message(STATUS "boost python library override specified: use this only to point to boost-python library for the correct python version if FindBoost doesn't find it")
	message(STATUS "overriding boost-python library with following library ${PYTHON_SCRIPT_UTIL_BOOST_PYTHON_LIBRARY_OVERRIDE}")
	set(Boost_PYTHON_LIBRARY "${PYTHON_SCRIPT_UTIL_BOOST_PYTHON_LIBRARY_OVERRIDE}")
endif()

set_property(TARGET boost-python PROPERTY IMPORTED_LOCATION ${Boost_PYTHON_LIBRARY})

message(STATUS "boost python library dir: ${Boost_LIBRARY_DIRS}")
message(STATUS "boost python libraries: ${Boost_LIBRARIES}")
message(STATUS "boost python headers: ${Boost_INCLUDE_DIRS}")

#
# Importing Python
#
find_package(PythonLibs 3.4 REQUIRED)
add_library(python SHARED IMPORTED)
set_property(TARGET python PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PYTHON_INCLUDE_DIRS})
set_property(TARGET python PROPERTY IMPORTED_LOCATION ${PYTHON_LIBRARIES})
include_directories(${PYTHON_INCLUDE_DIRS})

message(STATUS "python libraries: ${PYTHON_LIBRARIES}")
message(STATUS "python headers: ${PYTHON_INCLUDE_DIRS}")

add_library(python-script-util Source.cpp Run.cpp Scheduler.cpp Module.cpp System.cpp)

add_subdirectory(test)

enable_testing()
add_test(python-script-util-tests bin/python-script-util-tests)
