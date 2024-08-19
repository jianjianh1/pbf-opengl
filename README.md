# Position Based Fluids and Screen Space Fluid Rendering

A fluid simulation system based on Position Based Fluids and screen space rendering technique built from scratch with OpenGL.

## Demo

![Final Rendering](./demo/pdf-final.gif)

![Particle Only 1](./demo/pdf-particle.gif)

![Particle Only 2](./demo/pdf-particle2.gif)

## Build

### Dependencies

- `GLM`: already included in this repository
- `stb_image`: already included in this repository
- `GLFW`: needed to be installed; e.g., from the [official website](https://www.glfw.org/). The CMakeLists.txt should be able to find the library after installation
- Windows/Linux: macOS doesn't support the latest OpenGL including compute shader

### Build Script

#### Linux

The project's build system is cmake. To build the project, run the following shell command:

```bash
cmake -S . -B build
cd build
cmake --build .
```

After executing the commands, you be able to find executable `main`

#### Windows

You can use the same commands as above if you've activated Visual Studio environment in your terminal.

Alternatively, simply open the repository in Visual Studio and build it as usual.

## Run

Run the executable `main` or `main.exe` built as above.

### Control

- Arrow keys: control the boundary of the fluid
- `R` key: reset the fluid
- mouse drag: control the camera

## Reference

- [Position Based Fluids](https://doi.org/10.1145/2461912.2461984)
- [Screen Space Fluid Rendering for Games](https://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf)
- [Compute Shaders in Graphics: Gaussian Blur](https://lisyarus.github.io/blog/graphics/2022/04/21/compute-blur.htm)
- OpenGL Superbible: Comprehensive Tutorial and Reference
