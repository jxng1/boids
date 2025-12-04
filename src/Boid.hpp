#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Boid {
   public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;

    float maxForce;
    float maxSpeed;
    float perceptionRadius;
    float fovAngle;

    Boid(float x, float y);

    void applyForce(const sf::Vector2f& force);
    void update();
    void edges(int width, int height);
    void flock(const std::vector<Boid>& boids);
    void draw(sf::RenderWindow& window);

   private:
    // Helper functions
    static float mag(const sf::Vector2f& v);
    static sf::Vector2f normalize(const sf::Vector2f& v);
    static sf::Vector2f limit(const sf::Vector2f& v, float max);
    bool inFov(const Boid& other) const;

    sf::Vector2f align(const std::vector<Boid>& boids) const;
    sf::Vector2f cohesion(const std::vector<Boid>& boids) const;
    sf::Vector2f separation(const std::vector<Boid>& boids) const;
};
