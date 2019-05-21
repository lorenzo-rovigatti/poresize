CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(poresize)

OPTION(Debug "Set to ON to compile with debug symbols" OFF)
OPTION(G "Set to ON to compile with optimisations and debug symbols" OFF)

IF(CMAKE_VERSION VERSION_LESS "3.1")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ELSE()
	SET(CMAKE_CXX_STANDARD 14)
	SET(CMAKE_CXX_STANDARD_REQUIRED ON)
ENDIF()

IF(Debug)
	SET(CMAKE_BUILD_TYPE Debug)
	MESSAGE(STATUS "Compiling in Debug mode")
ENDIF(Debug)

IF(G)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo)
	MESSAGE(STATUS "Compiling in Release+Debug symbols mode")
ENDIF(G)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/extern )

SET(ps_SOURCES
	src/main.cpp
	src/System.cpp
)

ADD_EXECUTABLE(poresize ${ps_SOURCES})
