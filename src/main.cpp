#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <vector>

#include "AIBoid.hpp"
#include "EvolutionManager.hpp"
#include "FlockingMetrics.hpp"
#include "Globals.hpp"
#include "Predator.hpp"
#include "imgui-SFML.h"
#include "imgui.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({g_WIDTH, g_HEIGHT}), "Boids", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    if (ImGui::SFML::Init(window)) std::cout << "ImGui initialised successfully..." << std::endl;

    EvolutionConfig cfg;
    cfg.population = 80;
    cfg.generationTime = 15.f;
    NeuralNetwork candidate({10, 12, 2});
    EvolutionManager evo(cfg, candidate);
    evo.initialise();

    auto boids = evo.producePopulation();

    std::vector<Predator> preds;
    // preds.emplace_back(400, 300);

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::KeyPressed>() &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                g_drawFOV = !g_drawFOV;
            }
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, clock.getElapsedTime());
        ImGui::ShowDemoWindow();

        evo.update(dt);
        for (size_t i = 0; i < boids.size(); i++) {
            auto& boid = boids[i];

            boid.think(std::vector<Boid>(boids.begin(), boids.end()), preds, dt);
            boid.update();
            boid.edges(g_WIDTH, g_HEIGHT);

            float fitness = boid.computeFitness();
            evo.setFitness(i, fitness);
        }

        if (evo.generationComplete()) {
            boids = evo.producePopulation();

            FlockingMetrics metrics =
                computeFlockingMetrics(boids, g_aiBoidConfig.perceptionRadius);
            std::cout << "Generation " << evo.getGeneration()
                      << " | Avg Cohesion: " << metrics.averageCohesion
                      << " | Avg Alignment: " << metrics.averageAlignment << std::endl;
        }

        window.clear(sf::Color::Black);
        for (auto& b : boids) b.draw(window);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
