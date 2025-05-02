#include "Materials/Diffuse.h"
#include "Materials/Glossy.h"
#include "Scene/PointLight.h"
#include <RayTracer.h>
#include <exception>
#include <iostream>
#include <memory>


int main() {
    try {
        const uint32_t width = 600;
        const uint32_t height = 400;

        Materials::Diffuse floor_material ({0.6f, 0.6f, 0.2f, 1.0f});
        Materials::Glossy red   ({0.8f, 0.1f, 0.1f, 1.0f}, 0.1f);
        Materials::Glossy green ({0.1f, 0.8f, 0.1f, 1.0f}, 0.1f);
        Materials::Glossy blue  ({0.1f, 0.0f, 0.8f, 1.0f}, 0.1f);

        Materials::Dielectric glass (1.5f);
        glass.SetAbsorption(0.1f);
        Materials::Dielectric air (1.0f / 1.5f);
        air.SetAbsorption(0.0f);

        std::shared_ptr<Scene::Camera> camera = std::make_shared<Scene::Camera>(glm::vec3(0.0f, 1.0f,  2.0f));

        std::shared_ptr<Scene::Scene> scene = std::make_shared<Scene::Scene>();
        scene->SetCamera(camera);
        scene->AddLight<Scene::PointLight>(glm::vec3(0.0f, 4.0f, -5.0f), 0.01f);

        scene->Add<Geometry::Sphere>(glm::vec3(-2.0f, 0.5f, -6.0f), 1.0f, &green);
        scene->Add<Geometry::Sphere>(glm::vec3( 2.0f, 0.5f, -6.0f), 1.0f, &red);
        scene->Add<Geometry::Sphere>(glm::vec3( 0.0f, 1.0f, -10.0f), 1.0f, &blue);

        scene->Add<Geometry::Sphere>(glm::vec3(0.0f, -100.0f, -15.0f), 100.0f, &floor_material);

        scene->Add<Geometry::Sphere>(glm::vec3( -2.0f, 0.5f, -4.0f), 1.0f, &glass);
        scene->Add<Geometry::Sphere>(glm::vec3( -2.0f, 0.5f, -4.0f), 0.95f, &air);


        Application::RayTracer ray_tracer { width, height };
        ray_tracer.SetScene(scene);

        ray_tracer.Run();
    } catch (const std::exception &e) {
        std::cerr << "Uncaught exception: " << e.what() << "\n";
    }
    
    return 0;
}
