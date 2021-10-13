file(GLOB_RECURSE srcs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
list(REMOVE_ITEM srcs CMakeLists.txt)
install(FILES ${srcs} DESTINATION lib/vera++/scripts/rules)
