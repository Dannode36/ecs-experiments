#include <SFML/Graphics.hpp>
#include "ecs/ComponentArray.h"
#include "ecs/SparseSet.h"
#include "ecs/ecs.h"
#include <imgui.h>
#include <imgui-sfml.h>

constexpr auto PI = 3.14159265;

const int width = 800;
const int height = 400;
ecs::SparseSet<ecs::LivePixel> livePixelsSparse;
std::vector<ecs::Entity> entities;

void DrawEntities(sf::VertexArray& vertexArray, float entitySize) {
    
    auto& pixelsSparse = livePixelsSparse.data();

    // populate the vertex array, with two triangles per tile
    for (unsigned int i = 0; i < width; ++i) {
        for (unsigned int j = 0; j < height; ++j)
        {
            ecs::LivePixel livePixel = pixelsSparse[i + j * width];
            // get a pointer to the triangles' vertices of the current tile
            sf::Vertex * triangles = &vertexArray[(i + j * width) * 6];

            // define the 6 corners of the two triangles
            triangles[0].position = sf::Vector2f(i * entitySize, j * entitySize);
            triangles[1].position = sf::Vector2f((i + 1) * entitySize, j * entitySize);
            triangles[2].position = sf::Vector2f(i * entitySize, (j + 1) * entitySize);
            triangles[3].position = sf::Vector2f(i * entitySize, (j + 1) * entitySize);
            triangles[4].position = sf::Vector2f((i + 1) * entitySize, j * entitySize);
            triangles[5].position = sf::Vector2f((i + 1) * entitySize, (j + 1) * entitySize);

            // define the 6 matching texture coordinates
            triangles[0].color = livePixel.colour;
            triangles[1].color = livePixel.colour;
            triangles[2].color = livePixel.colour;
            triangles[3].color = livePixel.colour;
            triangles[4].color = livePixel.colour;
            triangles[5].color = livePixel.colour;
        }
    }
}

void UpdatePixelsSystem(float time) {
    auto& pixelsSparse = livePixelsSparse.data();

    for (auto& entity : entities)
    {
        auto pixel = livePixelsSparse.get(entity);
        //pixel->colour.a = (sin(time + pixel->offset) * 0.5f + 0.5f) * 255;
        pixel->colour.a = (fmod(time + pixel->offset, 360) * 0.5f + 0.5f) * 255;
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 800), "SFML works!");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    for (size_t i = 0; i < width * height; i++)
    {
        ecs::Entity id = ecs::newEntity();
        entities.push_back(id);
        livePixelsSparse.set(id, { sf::Color::Blue, static_cast<float>(rand()) });
    }

    sf::VertexArray vertexArray;
    vertexArray.setPrimitiveType(sf::Triangles);
    vertexArray.resize(livePixelsSparse.data().size() * 6);

    sf::Clock elapsedClock;
    sf::Time dtElapsed;
    sf::Clock dtClock;
    sf::Time dt;
    while (window.isOpen())
    {
        dt = dtClock.restart();
        dtElapsed += dt;

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        ImGui::SFML::Update(window, dt);

        ImGui::Begin("fps");
        ImGui::Text(std::to_string(1 / dt.asSeconds()).c_str());
        ImGui::End();

        UpdatePixelsSystem(dtElapsed.asSeconds());
        DrawEntities(vertexArray, 2.f);

        window.clear();
        window.draw(vertexArray);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
