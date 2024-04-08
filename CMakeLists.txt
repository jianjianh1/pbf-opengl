cmake_minimum_required(VERSION 3.20)
project(
    fluid-simulation
    VERSION 0.1.0
    LANGUAGES C CXX
    )

find_package(glfw3)
add_subdirectory("external")

add_subdirectory("src")

add_executable(main app/main.cpp "src/render/renderer.h" "src/render/renderer.cpp")
target_link_libraries(
    main PRIVATE
    renderer
    )
