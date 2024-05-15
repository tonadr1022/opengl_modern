# OpenGL Renderer 2 (4.6 AZDO edition)

Rather than using glDrawElements as I did in my [previous iteration](https://github.com/tonadr1022:opengl_renderer), my goal with this rendering engine has been to increase performance while desiging a more data oriented architecture. I use the [Entt](https://github.com/skypjack/entt) entity component system to manage scene data, rather than my previous object oriented approach that turned into a mess. Another improvement with this renderer has been avoiding premature abstraction with OpenGL concepts and decoupling the renderer from the rest of the app as much as possible, making iteration much simpler than the previous edition. The disorganization and object oriented nature of my previous renderer ultimately prevented me from implementing more techniques, and I realized starting fresh would allow me to make faster progress.

### Features

- Batched rendering using glMultiDrawElementsIndirect
- Bindless Textures
- In Progress: PBR

### TODO

- Anti-aliasing: MSAA/TSAA
- Post-processing
- Shadow pass
- Deferred rendering

### Dependencies

I use [Vcpkg](https://github.com/microsoft/vcpkg) for dependency management, and if Vcpkg is not installed locally when building with CMake, it is fetched and bootstrapped, installing these dependencies local to the repository.

- [GLFW](https://github.com/glfw/glfw) - cross platform window/input
- [GLM](https://github.com/icaven/glm) - math
- [Glew](https://github.com/nigels-com/glew) - OpenGL functions
- [Entt](https://github.com/skypjack/entt) - Fast Entity component system
- [Tracy](git@github.com:wolfpld/tracy.git) -Profiler (requires local install to use)
- [ImGui](https://github.com/ocornut/imgui) - UI
- [Spdlog](https://github.com/gabime/spdlog) - Logging
- [Assimp](https://github.com/assimp/assimp) - model loading

### References

- [LearnOpenGL](https://learnopengl.com/Advanced-Lighting/Deferred-Shading)
- [AZDO presentation](https://www.gdcvault.com/play/1020791/Approaching-Zero-Driver-Overhead-in)
- [Real-Time Rendering](https://www.realtimerendering.com)

random notes:

- pbr: null texture pointers that I was too dumb to notice due to moving unique pointer when amplacing into unordered map.
