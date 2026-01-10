set(
        UTIL_SOURCE
        CPPUtility.cpp
        cutility.c
)

add_library(util ${UTIL_SOURCE})
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
