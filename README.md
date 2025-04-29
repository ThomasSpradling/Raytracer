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

Items with a (*) are initially of lower priority and should be done last.

[ ] CPU Whitted Ray-tracer
    [ ] Set up platform layer with window creation and input handling
    [ ] Set up low-level Vulkan render directly to render a drawn-to image directly into swap-chain
    [ ] User interface and runtime controls
    [ ] Basic renderer which accumulates samples over time to converge to the correct image
    [ ] Handle several intersection methods against a `ray` interface
        [ ] Sphere
        [ ] Plane
        [ ] *Parametric Surfaces
        [ ] *Triangle Mesh
    [ ] Create scene graph hiearchy of items and camera to shoot initial rays
    [ ] Support more complicated materials
        [ ] Pure diffuse
        [ ] Pure reflective
        [ ] General dielectric materials with the Frensel equations
        [ ] Support Beer's law
    [ ] *More complicated light
        [ ] *Spot lights
        [ ] *Directional lights
    [ ] *Support barrel distortion and fish-eye lens
    [ ] *Texturing
    [ ] *Postprocessing
        [ ] *Gamma correction
        [ ] *Vignette
        [ ] *Chromatic aberration
    [ ] *Multi-threaded rendering system