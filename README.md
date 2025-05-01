# Raytracer

A simple renderer following a ray-tracing algorithm.

## Architecture
We use a layer-based architecture where the top layer depends on lower layers where lower layers need not have any implementation knowledge of how they are used by high layers. This allows us to seperate concerns in a modular way.

1. Application Layer (`/Core`): User interface settings and control
2. Rendering Layer (`/Renderer`): Implements ray-tracing algorithm and post-processing
3. Scene Layer (`/Scene`, `/Material`): Scene, camera, lights, materials, mediums, and texture
4. Geometry Layer (`/Geometry`): Primitive definitions, intersection tests, and acceleration structure
5. Low-level Platform Layer (`/Platform` and `/Backend`): Window creation, input handling, swapchain, graphics backend, and thread pools

## Roadmap

## Roadmap

Items marked with (*) are lower priority and should be tackled last.

- [ ] CPU Whitted Ray-tracer
    - [x] Set up platform layer (window creation + input handling)
    - [x] Add low-level Vulkan presenter that blits a rendered image into the swap chain
    - [ ] Implement user interface and runtime controls
    - [x] Basic renderer that accumulates samples over time until convergence
    - [ ] Intersection routines for the `Ray` interface  
        - [x] Sphere
        - [ ] Plane
        - [ ] *Parametric surfaces  
        - [ ] *Triangle mesh
    - [x] Scene rendering (objects + camera that emits primary rays)
    - [ ] Materials
        - [x] Pure diffuse  
        - [x] Pure reflective
        - [ ] Dielectric (Fresnel)  
        - [ ] Beer’s law absorption
    - [ ] Lighting
        - [x] Point lights  
        - [ ] *Spot lights  
        - [ ] *Directional lights
    - [ ] *Barrel distortion & fish-eye lens
    - [ ] *Texturing
    - [ ] *Post-processing  
        - [ ] *Gamma correction  
        - [ ] *Vignette  
        - [ ] *Chromatic aberration
    - [ ] *Multi-threaded rendering system
