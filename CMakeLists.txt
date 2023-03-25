cmake_minimum_required(VERSION 3.0.0)
project(open_aal VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_STATIC_LIBRARY_PREFIX "")

include(CTest)
enable_testing()

add_library(open_aal STATIC
    ./src/include/aal/amta.h
    ./src/include/aal/baatn.h
    ./src/include/aal/bagst.h
    ./src/include/aal/bars.h
    ./src/include/aal/barslist.h
    ./src/include/aal/baudc.h
    ./src/include/aal/blal.h
    ./src/amta.cpp
    ./src/bars.cpp
)

target_include_directories(open_aal PUBLIC ./src/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/ExtendedIO)
target_link_libraries(open_aal PUBLIC ExtendedIO)

set(BUILD_TESTING ON)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
