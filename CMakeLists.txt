cmake_minimum_required(VERSION 3.14)
project(Kahypar)

set(CMAKE_CXX_STANDARD 20)

add_executable(Kahypar main.cpp)

# macOS
IF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    # Apple Silicon
    IF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
        target_link_libraries(Kahypar ${CMAKE_SOURCE_DIR}/macOS/AppleSilicon/libkahypar.a)
        target_link_libraries(Kahypar ${CMAKE_SOURCE_DIR}/macOS/AppleSilicon/libmini_boost.a)
    # Intel
    ELSE ()
        message("Operating system: macOS (Intel)")
        message(FATAL_ERROR "KaHyPar is not supported!")
    ENDIF ()

# Windows
ELSEIF ((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") OR (CMAKE_SYSTEM_NAME STREQUAL "CYGWIN"))
    message("Operating system: Windows")
    target_link_libraries(Kahypar ${CMAKE_SOURCE_DIR}/Windows/libkahypar.a)
    target_link_libraries(Kahypar ${CMAKE_SOURCE_DIR}/Windows/libmini_boost.a)

# Linux
ELSEIF (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message("Operating system: Linux")
    target_link_libraries(Kahypar ${CMAKE_SOURCE_DIR}/Linux/libkahypar.a)
    target_link_libraries(Kahypar ${CMAKE_SOURCE_DIR}/Linux/libmini_boost.a)

# Undefined
ELSE ()
    message(FATAL_ERROR "Your operating system (" ${CMAKE_SYSTEM_NAME} ") is not supported!")
ENDIF ()
