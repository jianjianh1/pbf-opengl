add_subdirectory(glutils)

target_include_directories(shader_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shader_program PUBLIC glad)