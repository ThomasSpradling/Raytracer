#include <RayTracer.h>
#include <exception>
#include <iostream>
#include <memory>


int main() {
    try {
        const uint32_t width = 600;
        const uint32_t height = 400;

        Materials::Diffuse green({0.1f, 0.8f, 0.1f, 1.0f});
        Materials::Diffuse red  ({0.8f, 0.1f, 0.1f, 1.0f});
        Materials::Metal metal ({ 0.5f, 0.5f, 0.5f, 1.0f });

        std::shared_ptr<Scene::Camera> camera = std::make_shared<Scene::Camera>();

        std::shared_ptr<Scene::Scene> scene = std::make_shared<Scene::Scene>();
        scene->SetCamera(camera);
        scene->Add<Geometry::Sphere>(glm::vec3(-1.5f, 0.0f, -5.0f), 1.0f, &green);
        for (uint32_t i = 0; i < 100; ++i) {
            scene->Add<Geometry::Sphere>(glm::vec3(0.0f, 0.0f, -8.0f), 1.0f, &metal);
        }
        scene->Add<Geometry::Sphere>(glm::vec3(1.5f, 0.0f, -5.0f), 1.0f, &red);

        Application::RayTracer ray_tracer { width, height };
        ray_tracer.SetScene(scene);

        ray_tracer.Run();
    } catch (const std::exception &e) {
        std::cerr << "Uncaught exception: " << e.what() << "\n";
    }
    
    return 0;
}
