<div align="center">

  # Photon renderer
</div>

![Example Render produced by Photon renderer](./images/cover_image.png)

### About
Photon is a spectral physically based CPU renderer written in C++. It is based on the first part of [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html). It supports brute force path tracing with as many threads as your computer offers and got a little bit neglected, because I switched my focus to GPU programming. However, it is a great playground and I am planning on implementing a few different rendering algorithms with it.

Originally, it was implemented on Windows, but I did not test it for quite some time there, it works on Linux though. The added google test was primarily added to get to know how to write tests with C++; so, do not get fooled into thinking this projects is greatly tested 😀

### Features
* physically based lighting with spectral dispersion at translucent surfaces
* capable of multithreading (thread count can be set at the top of main.cpp in the constructor call of RayTracer)
* shows live preview of rendering progress
* supports incremental rendering mode where samples are accumulated over time (better for preview), but this mode is slower

### Dependencies
As this project was originally developed on Windows most dependencies are included with the project.
#### external
* SDL2
#### included
* glm
* stb
* Dear ImGui
* tinyobj

