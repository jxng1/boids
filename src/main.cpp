#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <vector>

#include "Boid.hpp"

int main() {
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Boids", sf::State::Fullscreen);
    window.setFramerateLimit(60);

    std::vector<Boid> boids;
    for (int i = 0; i < 50; i++) {
        boids.emplace_back(Boid(rand() % WIDTH, rand() % HEIGHT));
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        for (auto& b : boids) {
            b.flock(boids);
        }

        for (auto& b : boids) {
            b.update();
            b.edges(WIDTH, HEIGHT);
        }

        window.clear(sf::Color::Black);

        for (auto& b : boids) b.draw(window);

        window.display();
    }

    return 0;
}
