add_subdirectory("misc")
target_include_directories(bounding_box PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bounding_box PUBLIC glm)
target_include_directories(grid INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(grid INTERFACE glm)
target_include_directories(helper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(image PUBLIC stb)
target_include_directories(image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("glutils")
target_include_directories(shader_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shader_program PUBLIC glad glm)
target_include_directories(ssbo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ssbo PUBLIC glad)
target_include_directories(vao PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vao PUBLIC glad ssbo)
target_include_directories(texture PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(texture PUBLIC glad glm)
target_include_directories(fbo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(fbo PUBLIC glad texture)
target_include_directories(cubemap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cubemap PUBLIC glad image)

add_subdirectory("simulation")
target_include_directories(fluid_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    fluid_system PUBLIC
    glm
    glad
    helper
    bounding_box
    ssbo
    vao
    shader_program
    )

add_subdirectory("render")
target_link_libraries(orbit_camera PUBLIC glm)
target_link_libraries(orbit_light PUBLIC glm)
target_link_libraries(fullscreen_quad PUBLIC glad shader_program vao)
target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    renderer PUBLIC
    glad
    glfw
    glm
    grid
    orbit_camera
    orbit_light
    fluid_system
    texture
    fbo
    fullscreen_quad
    cubemap
    )
