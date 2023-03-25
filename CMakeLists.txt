cmake_minimum_required(VERSION 3.0.0)
project(aal VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(aal STATIC
    ./src/include/amta.h
    ./src/include/baatn.h
    ./src/include/bagst.h
    ./src/include/bars.h
    ./src/include/barslist.h
    ./src/include/baudc.h
    ./src/include/blal.h
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
