
include_directories("${CMAKE_CURRENT_LIST_DIR}/inc")

FILE(GLOB EMLIB_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.c")

add_library(emlib ${EMLIB_SOURCES})
