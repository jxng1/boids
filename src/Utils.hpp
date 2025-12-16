#pragma once
#include <SFML/System.hpp>
#include <cmath>

// Compute Euclidean distance between two 2D points
inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Magnitude of a vector
inline float mag(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

// Normalise a vector
inline sf::Vector2f normalise(const sf::Vector2f& v) {
    float l = mag(v);
    if (l == 0.f) return sf::Vector2f(0.f, 0.f);
    return v / l;
}

// Dot product of two vectors
inline float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}