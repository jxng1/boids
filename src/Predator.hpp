#pragma once
#include <SFML/Graphics.hpp>

struct Predator {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed = 140.f;

    Predator(float x = 0.f, float y = 0.f) {
        position = {x, y};
        velocity = {0.f, 0.f};
    }

    void update(float dt) {
        position += velocity * dt;
    }

    void draw(sf::RenderWindow& window) {
        sf::CircleShape s(8.f);
        s.setOrigin(sf::Vector2f(8.f, 8.f));
        s.setPosition(position);
        s.setFillColor(sf::Color::Magenta);
        window.draw(s);
    }
};
