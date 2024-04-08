add_subdirectory("misc")
target_include_directories(parameters INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(parameters INTERFACE glm)
target_include_directories(bounding_box PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bounding_box PUBLIC glm)
target_include_directories(grid INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(grid INTERFACE glm)

add_subdirectory("glutils")
target_include_directories(shader_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shader_program PUBLIC glad)

add_subdirectory("simulation")
target_include_directories(fluid_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    fluid_system PUBLIC
    glm
    glad
    parameters
    bounding_box
    shader_program
    )

add_subdirectory("render")
target_link_libraries(orbit_camera PUBLIC glm)
target_link_libraries(orbit_light PUBLIC glm)
target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    renderer PUBLIC
    glad
    glfw
    parameters
    grid
    )
