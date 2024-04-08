#include <render/renderer.h>

#include <iostream>

int main()
{
    Renderer renderer(800, 600, "Fluid Simulation");

    int maxWorkGroupSize[3]{};
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
    std::cout << "Maximal work group size x: " << maxWorkGroupSize[0] << '\n';

    return 0;
}