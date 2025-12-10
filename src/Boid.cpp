#include "Boid.hpp"

#include <cmath>
#include <cstdlib>

#include "Utils.hpp"

// Constructor
Boid::Boid(float x, float y) {
    position = sf::Vector2f(x, y);
    velocity =
        sf::Vector2f((float(rand() % 100) / 100.f) - 0.5f, (float(rand() % 100) / 100.f) - 0.5f);
    acceleration = sf::Vector2f(0, 0);
    maxForce = 0.05f;
    maxSpeed = 2.f;
    perceptionRadius = 50.f;
    fovAngle = 180.f;
}

void Boid::applyForce(const sf::Vector2f& force) {
    acceleration += force;
}

void Boid::update() {
    velocity += acceleration;
    velocity = limit(velocity, maxSpeed);
    position += velocity;
    acceleration *= 0.f;
}

void Boid::edges(int width, int height) {
    if (position.x < 0) position.x = width;
    if (position.x > width) position.x = 0;
    if (position.y < 0) position.y = height;
    if (position.y > height) position.y = 0;
}

void Boid::flock(const std::vector<Boid>& boids) {
    sf::Vector2f alignment = align(boids);
    sf::Vector2f coh = cohesion(boids);
    sf::Vector2f sep = separation(boids);

    applyForce(alignment);
    applyForce(coh);
    applyForce(sep);
}

void Boid::draw(sf::RenderWindow& window) {
    sf::ConvexShape triangle;
    triangle.setPointCount(3);

    triangle.setPoint(0, sf::Vector2f(0.f, -6.f));  // tip
    triangle.setPoint(1, sf::Vector2f(-4.f, 4.f));  // bottom left
    triangle.setPoint(2, sf::Vector2f(4.f, 4.f));   // bottom right

    float angle = std::atan2(velocity.y, velocity.x) * 180.f / 3.14159f + 90.f;  // orientate
    triangle.setRotation(sf::degrees(angle));
    triangle.setPosition(position);
    triangle.setFillColor(sf::Color::Red);

    window.draw(triangle);
}

sf::Vector2f Boid::limit(const sf::Vector2f& v, float max) {
    float m = mag(v);
    if (m > max) return sf::Vector2f(v.x / m * max, v.y / m * max);
    return v;
}

sf::Vector2f Boid::align(const std::vector<Boid>& boids) const {
    float perception = 50.f;
    sf::Vector2f steering(0, 0);
    int total = 0;

    for (const Boid& other : boids) {
        float d = mag(position - other.position);
        if (&other != this && d < perception) {
            steering += other.velocity;
            total++;
        }
    }

    if (total > 0) {
        steering /= (float)total;
        steering = normalise(steering) * maxSpeed;
        steering -= velocity;
        steering = limit(steering, maxForce);
    }

    return steering;
}

sf::Vector2f Boid::cohesion(const std::vector<Boid>& boids) const {
    float perception = 50.f;
    sf::Vector2f steering(0, 0);
    int total = 0;

    for (const Boid& other : boids) {
        float d = mag(position - other.position);
        if (&other != this && d < perception) {
            steering += other.position;
            total++;
        }
    }

    if (total > 0) {
        steering /= (float)total;
        steering -= position;
        steering = normalise(steering) * maxSpeed;
        steering -= velocity;
        steering = limit(steering, maxForce);
    }

    return steering;
}

sf::Vector2f Boid::separation(const std::vector<Boid>& boids) const {
    float perception = 25.f;
    sf::Vector2f steering(0, 0);
    int total = 0;

    for (const Boid& other : boids) {
        float d = mag(position - other.position);
        if (&other != this && d < perception) {
            sf::Vector2f diff = position - other.position;
            diff /= d * d;  // stronger repulsion when closer
            steering += diff;
            total++;
        }
    }

    if (total > 0) {
        steering /= (float)total;
        steering = normalise(steering) * maxSpeed;
        steering -= velocity;
        steering = limit(steering, maxForce);
    }

    return steering;
}