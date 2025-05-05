#include "Scene/PointLight.h"
#include <RayTracer.h>
#include <exception>
#include <iostream>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

inline void LoadModel(const std::string &filename,
                      Scene::Scene &scene,
                      std::shared_ptr<Materials::Material> material,
                      const glm::mat4 &modelToWorld = glm::mat4(1.0f))
{
    Assimp::Importer importer;
    const aiScene *aiscene = importer.ReadFile(
        filename,
        aiProcess_Triangulate
      | aiProcess_FlipUVs
      | aiProcess_GenSmoothNormals
    );
    if (!aiscene || !aiscene->HasMeshes())
        throw std::runtime_error("Failed to load model: " + filename);

    for (unsigned m = 0; m < aiscene->mNumMeshes; ++m) {
        const aiMesh *aiMesh = aiscene->mMeshes[m];

        std::vector<glm::vec3> positions;
        positions.reserve(aiMesh->mNumVertices);
        for (unsigned i = 0; i < aiMesh->mNumVertices; ++i) {
            auto &v = aiMesh->mVertices[i];
            glm::vec4 p{v.x, v.y, v.z, 1.0f};
            p = modelToWorld * p;                   // **apply our transform**
            positions.emplace_back(p.x, p.y, p.z);
        }

        std::vector<glm::vec2> uvs;
        if (aiMesh->HasTextureCoords(0)) {
            uvs.reserve(aiMesh->mNumVertices);
            for (unsigned i = 0; i < aiMesh->mNumVertices; ++i) {
                auto &t = aiMesh->mTextureCoords[0][i];
                uvs.emplace_back(t.x, t.y);
            }
        }

        std::vector<uint32_t> indices;
        indices.reserve(aiMesh->mNumFaces*3);
        for (unsigned f = 0; f < aiMesh->mNumFaces; ++f) {
            auto &face = aiMesh->mFaces[f];
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        auto mesh = std::make_unique<Geometry::TriangleMesh>(material);
        mesh->SetVertices(positions);
        if (!uvs.empty()) mesh->SetUVs(uvs);
        mesh->SetIndices(indices);
        scene.Add(std::move(mesh));
    }
}

std::shared_ptr<Scene::Scene> BasicTriangleScene() {
    auto floor_material = std::make_shared<Materials::Checkerboard>(
        glm::vec4{0.2f, 0.2f, 0.2f, 1.0f},
        glm::vec4{0.8f, 0.8f, 0.8f, 1.0f},
        100.0f
    );

    auto blue = std::make_shared<Materials::Glossy>(
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f},
        0.95f,  
        glm::vec4{1.0f, 0.85f, 0.57f, 1.0f}
    );

    auto green = std::make_shared<Materials::Glossy>(
        glm::vec4{0.1f, 0.8f, 0.1f, 1.0f}, 
        0.1f
    );

    auto gold = std::make_shared<Materials::Dielectric>(1.5f, 0.1f, 0.2f);
    gold->SetAbsorption(0.05f);

    auto triangle_material = std::make_shared<Materials::Dielectric>(
        1.5f, 0.1f, 0.2f
    );
    triangle_material->SetAbsorption(0.05f);

    auto glass = std::make_shared<Materials::Dielectric>(1.5f, 0.1f, 0.2f);
    glass->SetAbsorption(0.10f);

    auto air = std::make_shared<Materials::Dielectric>(1.0f / 1.5f);
    air->SetAbsorption(0.0f);

    std::shared_ptr<Scene::Camera> camera = std::make_shared<Scene::Camera>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    std::shared_ptr<Scene::Scene> scene = std::make_shared<Scene::Scene>();
    scene->SetCamera(camera);
    scene->AddLight<Scene::PointLight>(glm::vec3(0.0f, 5.0f, -5.0f), Color(100.0f, 100.0f, 100.0f, 1.0f));

    {
        // glm::mat4 transform = glm::translate(glm::mat4(1.0f),
        //                                    glm::vec3(0, 0.1f, -5.0f))
        //                  * glm::scale(glm::mat4(1.0f),
        //                              glm::vec3(0.5f));
        // LoadModel(STRONK_RESOURCE_PATH "/models/utah_teapot.obj", *scene, &triangle_material, transform);
        auto triangle_mesh = std::make_unique<Geometry::TriangleMesh>(triangle_material);
        triangle_mesh->SetVertices({
            { -0.5f,  -0.5f, -3.0f },
            {  0.5f,  -0.5f, -3.0f },
            {  0.0f,  0.5f, -3.0f }
        });
        triangle_mesh->SetUVs({
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 0.5f, 1.0f }
        });
        triangle_mesh->SetIndices({ 0, 1, 2});
        scene->Add(std::move(triangle_mesh));
    }

    scene->Add<Geometry::Sphere>(glm::vec3(-2.0f, 0.5f, -6.0f), 1.0f, green);
    scene->Add<Geometry::Sphere>(glm::vec3(-2.0f, 0.5f, -6.0f), 1.0f, green);
    scene->Add<Geometry::Sphere>(glm::vec3( 2.0f, 0.3f, -6.5f), 0.7f, gold);
    scene->Add<Geometry::Sphere>(glm::vec3( 0.0f, 1.0f, -10.0f), 1.0f, blue);

    scene->Add<Geometry::Sphere>(glm::vec3(0.0f, -100.0f, -15.0f), 100.0f, floor_material);

    scene->Add<Geometry::Sphere>(glm::vec3( -2.0f, 0.5f, -4.0f), 1.0f, glass);
    scene->Add<Geometry::Sphere>(glm::vec3( -2.0f, 0.5f, -4.0f), 0.95f, air);

    return scene;
}

std::shared_ptr<Scene::Scene> Scene0() {
    std::shared_ptr<Scene::Scene> scene = std::make_shared<Scene::Scene>();

    {
        auto camera = std::make_shared<Scene::Camera>(
            glm::vec3(0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            45.0f,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    
        scene->SetCamera(camera);
    }

    {
        auto red = std::make_shared<Materials::Diffuse>(Color(0.75f, 0.25f, 0.25f, 1.0f));
        auto blue = std::make_shared<Materials::Diffuse>(Color(0.25f, 0.25f, 0.75f, 1.0f));

        scene->Add<Geometry::Sphere>(glm::vec3(0.0f, 0.0f, -3.0f), 0.5f, red);
        scene->Add<Geometry::Sphere>(glm::vec3(0.0f, -100.5f, 3.0f), 100.0f, blue);

        scene->AddLight<Scene::PointLight>(glm::vec3(5.0f, 5.0f, -2.0f), Color(100.0f, 100.0f, 100.0f, 1.0f));
    }
    return scene;
}

std::shared_ptr<Scene::Scene> Scene1() {
    std::shared_ptr<Scene::Scene> scene = std::make_shared<Scene::Scene>();

    {
        auto camera = std::make_shared<Scene::Camera>(
            glm::vec3(0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            45.0f,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    
        scene->SetCamera(camera);
    }

    {
        auto yellow = std::make_shared<Materials::Diffuse>(Color(0.8f, 0.8f, 0.2f, 1.0f));
        auto red = std::make_shared<Materials::Diffuse>(Color(0.75f, 0.25f, 0.25f, 1.0f));
        auto magenta = std::make_shared<Materials::Mirror>(Color(0.75f, 0.25f, 0.75f, 1.0f));
        auto teal = std::make_shared<Materials::Mirror>(Color(0.25f, 0.75f, 0.75f, 1.0f));

        scene->Add<Geometry::Sphere>(glm::vec3(0.0f, -100.5f, -3.0f), 100.0f, yellow);
        scene->Add<Geometry::Sphere>(glm::vec3(0.0f, 0.0f, -3.0f), 0.5f, red);
        scene->Add<Geometry::Sphere>(glm::vec3(1.0f, 0.0f, -3.0f), 0.5f, magenta);
        scene->Add<Geometry::Sphere>(glm::vec3(-1.0f, 0.0f, -3.0f), 0.5f, teal);

        scene->AddLight<Scene::PointLight>(glm::vec3(5.0f, 5.0f, 2.0f), Color(100.0f, 100.0f, 100.0f, 1.0f));
        scene->AddLight<Scene::PointLight>(glm::vec3(-5.0f, 5.0f, 1.0f), Color(10.0f, 10.0f, 10.0f, 1.0f));
        scene->AddLight<Scene::PointLight>(glm::vec3(0.0f, 5.0f, -5.0f), Color(2.0f, 2.0f, 2.0f, 1.0f));
    }
    return scene;
}

std::shared_ptr<Scene::Scene> Scene2() {
    std::shared_ptr<Scene::Scene> scene = std::make_shared<Scene::Scene>();

    {
        auto camera = std::make_shared<Scene::Camera>(
            glm::vec3(0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            45.0f,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    
        scene->SetCamera(camera);
    }

    {
        auto yellow = std::make_shared<Materials::Mirror>(Color(0.75, 0.75, 0.25, 1.0f));
        auto teal = std::make_shared<Materials::Diffuse>(Color(0.25, 0.75, 0.75, 1.0f));
        auto magenta = std::make_shared<Materials::Mirror>(Color(0.75, 0.25, 0.75, 1.0f));

        scene->Add<Geometry::Sphere>(glm::vec3(-0.75, 0.0,  -4.0), 1.0f, yellow);
        scene->Add<Geometry::Sphere>(glm::vec3(1.0, 0.0, -13.0), 7.5f, teal);
        scene->Add<Geometry::Sphere>(glm::vec3(0.5, 0.0,  -3.0), 0.25f, magenta);

        scene->AddLight<Scene::PointLight>(glm::vec3(1, 0, 10), Color(100, 50, 50, 1.0f));
        scene->AddLight<Scene::PointLight>(glm::vec3(-1, 0, 10), Color(50, 50, 100, 1.0f));
    }
    return scene;
}

std::shared_ptr<Scene::Scene> Scene3() {
    auto scene = std::make_shared<Scene::Scene>();

    // -- camera
    {
        auto cam = std::make_shared<Scene::Camera>(
            glm::vec3{0.0f},                // look_from
            glm::vec3{0.0f,0.0f,-1.0f},     // look_at
            45.0f,                          // field_of_view
            glm::vec3{0.0f,1.0f,0.0f}       // up
        );
        scene->SetCamera(cam);
    }

    // -- materials
    auto m0 = std::make_shared<Materials::Diffuse>(glm::vec4{0.5f, 0.25f, 0.25f, 1.0f});
    auto m1 = std::make_shared<Materials::Diffuse>(glm::vec4{0.25f,0.5f, 0.75f, 1.0f});
    auto m2 = std::make_shared<Materials::Diffuse>(glm::vec4{0.75f,0.5f, 0.25f, 1.0f});
    auto m3 = std::make_shared<Materials::Mirror >(glm::vec4{0.25f,0.75f,0.5f, 1.0f});
    auto m4 = std::make_shared<Materials::Diffuse>(glm::vec4{0.5f, 0.75f,0.5f, 1.0f});
    auto m5 = std::make_shared<Materials::Mirror >(glm::vec4{0.5f, 0.5f, 0.75f,1.0f});
    auto m6 = std::make_shared<Materials::Diffuse>(glm::vec4{0.5f, 0.5f, 0.75f,1.0f});
    auto m7 = std::make_shared<Materials::Diffuse>(glm::vec4{0.75f,0.75f,0.75f,1.0f});

    std::vector<std::shared_ptr<Materials::Material>> mats = { m0,m1,m2,m3,m4,m5,m6,m7 };

    // -- spheres
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.0f,  0.0f,  0.0f}, 100.0f, mats[0]);
    scene->Add<Geometry::Sphere>(glm::vec3{-0.35f, 0.35f,-3.5f},  0.25f, mats[1]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.35f, 0.35f,-2.5f},  0.35f, mats[2]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.35f,-0.35f,-2.0f},  0.30f, mats[3]);
    scene->Add<Geometry::Sphere>(glm::vec3{-0.35f,-0.35f,-4.0f},  0.325f,mats[4]);
    scene->Add<Geometry::Sphere>(glm::vec3{-1.5f, 0.0f, -3.0f},  0.50f, mats[5]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.5f, 0.0f, -3.0f},  0.50f, mats[6]);
    scene->Add<Geometry::Sphere>(glm::vec3{10.0f, 0.0f, -3.0f},  0.50f, mats[7]);
    scene->Add<Geometry::Sphere>(glm::vec3{-10.0f,0.0f, -3.0f},  0.50f, mats[7]);  // same material as 7

    // -- lights
    // position then intensity
    scene->AddLight<Scene::PointLight>(
        glm::vec3{0.0f,0.0f,0.0f},
        glm::vec4{10.0f,10.0f,10.0f,1.0f}
    );
    scene->AddLight<Scene::PointLight>(
        glm::vec3{-0.4f,0.5f,-3.0f},
        glm::vec4{0.5f, 0.5f, 0.5f,1.0f}
    );
    scene->AddLight<Scene::PointLight>(
        glm::vec3{0.0f,0.0f,90.0f},
        glm::vec4{10000.0f,10000.0f,10000.0f,1.0f}
    );

    return scene;
}

std::shared_ptr<Scene::Scene> Scene4() {
    auto scene = std::make_shared<Scene::Scene>();

    // -- camera
    {
        auto cam = std::make_shared<Scene::Camera>(
            glm::vec3{0.0f}, glm::vec3{0.0f,0.0f,-1.0f},
            45.0f, glm::vec3{0.0f,1.0f,0.0f}
        );
        scene->SetCamera(cam);
    }

    // -- materials (30 total)
    //   use Diffuse or Mirror as per the reference list
    std::vector<std::shared_ptr<Materials::Material>> mats;
    mats.reserve(30);
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.020f,0.660f,0.021f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.823f,0.830f,0.703f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.471f,0.540f,0.414f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.997f,0.048f,0.431f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.253f,0.089f,0.712f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.664f,0.884f,0.069f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.399f,0.475f,0.090f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.360f,0.298f,0.956f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.147f,0.115f,0.440f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.881f,0.312f,0.609f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.946f,0.094f,0.617f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.649f,0.847f,0.018f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.994f,0.240f,0.637f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.228f,0.861f,0.613f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.442f,0.546f,0.580f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.122f,0.874f,0.081f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.954f,0.575f,0.910f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.601f,0.420f,0.757f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.340f,0.136f,0.233f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.227f,0.570f,0.241f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.906f,0.774f,0.042f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.287f,0.709f,0.301f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.895f,0.787f,0.824f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.011f,0.395f,0.117f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.781f,0.390f,0.375f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.155f,0.873f,0.695f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.276f,0.751f,0.104f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.503f,0.465f,0.232f,1.0f}));
    mats.push_back(std::make_shared<Materials::Diffuse>(glm::vec4{0.264f,0.794f,0.280f,1.0f}));
    mats.push_back(std::make_shared<Materials::Mirror> (glm::vec4{0.036f,0.548f,0.363f,1.0f}));

    // -- spheres (30 of them)
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.781f, 2.293f,-4.602f}, 0.659f, mats[ 0]);
    scene->Add<Geometry::Sphere>(glm::vec3{-1.975f,-1.115f,-5.906f}, 0.591f, mats[ 1]);
    scene->Add<Geometry::Sphere>(glm::vec3{-0.900f,-0.518f,-4.741f}, 0.632f, mats[ 2]);
    scene->Add<Geometry::Sphere>(glm::vec3{-2.281f, 0.900f,-4.271f}, 0.392f, mats[ 3]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.309f, 2.047f,-6.365f}, 0.550f, mats[ 4]);
    scene->Add<Geometry::Sphere>(glm::vec3{-1.281f, 2.314f,-4.383f}, 0.415f, mats[ 5]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.229f,-0.093f,-3.150f}, 0.331f, mats[ 6]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.400f, 1.793f,-3.364f}, 0.322f, mats[ 7]);
    scene->Add<Geometry::Sphere>(glm::vec3{-0.795f,-2.459f,-6.424f}, 0.563f, mats[ 8]);
    scene->Add<Geometry::Sphere>(glm::vec3{-1.045f,-1.886f,-4.895f}, 0.418f, mats[ 9]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 2.369f,-0.402f,-5.811f}, 0.510f, mats[10]);
    scene->Add<Geometry::Sphere>(glm::vec3{-0.514f, 2.270f,-2.799f}, 0.635f, mats[11]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.669f, 1.321f,-7.112f}, 0.300f, mats[12]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.227f, 1.167f,-5.870f}, 0.299f, mats[13]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.320f, 0.086f,-7.343f}, 0.510f, mats[14]);
    scene->Add<Geometry::Sphere>(glm::vec3{-1.410f,-1.046f,-2.946f}, 0.280f, mats[15]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.742f,-1.488f,-6.187f}, 0.517f, mats[16]);
    scene->Add<Geometry::Sphere>(glm::vec3{-1.867f, 0.315f,-4.901f}, 0.747f, mats[17]);
    scene->Add<Geometry::Sphere>(glm::vec3{-0.269f, 0.208f,-4.738f}, 0.431f, mats[18]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.966f, 0.860f,-5.417f}, 0.290f, mats[19]);
    scene->Add<Geometry::Sphere>(glm::vec3{-2.345f, 1.133f,-5.147f}, 0.298f, mats[20]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.924f, 1.527f,-6.724f}, 0.510f, mats[21]);
    scene->Add<Geometry::Sphere>(glm::vec3{-0.418f,-1.651f,-2.595f}, 0.594f, mats[22]);
    scene->Add<Geometry::Sphere>(glm::vec3{-2.344f,-0.415f,-3.685f}, 0.617f, mats[23]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.238f, 0.990f,-3.018f}, 0.306f, mats[24]);
    scene->Add<Geometry::Sphere>(glm::vec3{-1.016f,-2.252f,-2.555f}, 0.392f, mats[25]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.761f, 1.176f,-5.583f}, 0.407f, mats[26]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.995f, 2.143f,-2.525f}, 0.296f, mats[27]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 1.645f,-1.920f,-3.988f}, 0.361f, mats[28]);
    scene->Add<Geometry::Sphere>(glm::vec3{ 0.189f,-0.262f,-4.485f}, 0.298f, mats[29]);

    // -- lights
    scene->AddLight<Scene::PointLight>(
      glm::vec3{ 0.0f },
      glm::vec4{ 10.0f, 10.0f, 10.0f, 1.0f}
    );
    scene->AddLight<Scene::PointLight>(
      glm::vec3{  5.0f,  5.0f, -5.0f },
      glm::vec4{  50.0f, 5.0f, 5.0f, 1.0f }
    );

    return scene;
}

int main(int argc, char **argv) {
    try {
        const uint32_t width = 600;
        const uint32_t height = 400;
        const uint32_t samples_per_pixel = 32;

        // Parse args

        bool no_gui = false;
        std::string output_file = "";
        std::string valid_usage_str = "\tUsage raytracer [--nogui] [--output filename]";
        for (int i = 1; i < argc; ++i) {
            if (std::strcmp(argv[i], "--gui") == 0) {

            } else if (std::strcmp(argv[i], "--output") == 0) {
                if (i + 1 >= argc) {
                    std::cerr << "" << argv[i] << std::endl;
                    std::cerr << valid_usage_str << argv[i] << std::endl;
                    exit(1);
                }

                output_file = argv[i + 1];
                i++;
            } else {
                std::cerr << "Unknown argument: " << argv[i] << std::endl;
                std::cerr << valid_usage_str << std::endl;
                exit(1);
            }
        }

        Application::RayTracer ray_tracer { width, height, samples_per_pixel };
        std::shared_ptr<Scene::Scene> scene;
        {
            PROFILE_SCOPE_AUTO("Scene Creation");
            scene = BasicTriangleScene();
        }

        if (!output_file.empty()) {
            ray_tracer.SetOutputPath(output_file);
        }

        ray_tracer.SetScene(scene);
        ray_tracer.Run();
    } catch (const std::exception &e) {
        std::cerr << "Uncaught exception: " << e.what() << "\n";
    }
    
    return 0;
}
