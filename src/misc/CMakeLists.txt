add_library(parameters INTERFACE)

add_library(grid INTERFACE)

add_library(bounding_box "bounding_box.cpp" "bounding_box.h" "parameters.h" "grid.h" "helper.h" "helper.cpp")

add_library(helper "helper.cpp" "helper.h")