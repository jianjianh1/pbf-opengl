#include <render/renderer.h>

#include <iostream>

int main()
{
    Renderer renderer{};

    int maxWorkGroupSize[3]{};
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
    std::cout << "Maximum work group size x: " << maxWorkGroupSize[0] << '\n';

    int maxWorkGroupInvocations{};
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxWorkGroupInvocations);
    std::cout << "Maximum work group invocations: " << maxWorkGroupInvocations << '\n';

    renderer.run();

    return 0;
}