#include <RayTracer.h>
#include <exception>
#include <iostream>


int main() {
    try {
        const uint32_t width = 512;
        const uint32_t height = 512;

        Application::RayTracer ray_tracer { width, height };
        ray_tracer.Run();
    } catch (const std::exception &e) {
        std::cerr << "Uncaught exception: " << e.what() << "\n";
    }
    
    return 0;
}
