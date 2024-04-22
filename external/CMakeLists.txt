add_subdirectory("glm")
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("glad")

add_subdirectory("stb")
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
