cmake_minimum_required(VERSION 3.20)
project(
  fluid-simulation
  VERSION 0.1.0
  LANGUAGES C CXX
  )

add_subdirectory("external")

find_package(glfw3)

add_executable(main app/main.cpp)
target_link_libraries(
  main PRIVATE
  glfw
  glm
  glad
  )
