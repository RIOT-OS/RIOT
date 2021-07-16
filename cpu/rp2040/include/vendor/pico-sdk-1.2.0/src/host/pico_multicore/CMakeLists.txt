if (NOT TARGET pico_multicore)
    pico_add_impl_library(pico_multicore)

    target_include_directories(pico_multicore INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()



