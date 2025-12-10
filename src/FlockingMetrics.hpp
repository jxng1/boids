#pragma once
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

#include "AIBoid.hpp"
#include "Utils.hpp"

struct FlockingMetrics {
    float averageCohesion = 0.f;   // average distance to neighbours
    float averageAlignment = 0.f;  // average alignment [-1, 1]
};

// Compute flocking metrics for a group of boids
inline FlockingMetrics computeFlockingMetrics(const std::vector<AIBoid>& boids,
                                              float perceptionRadius = 50.f) {
    FlockingMetrics metrics;

    float totalCohesion = 0.f;
    float totalAlignment = 0.f;
    int count = 0;

    for (const auto& b : boids) {
        sf::Vector2f avgVel(0.f, 0.f);
        float avgDist = 0.f;
        int neighbours = 0;

        for (const auto& other : boids) {
            if (&b == &other) continue;
            float d = distance(b.position, other.position);
            if (d < perceptionRadius) {
                avgVel += other.velocity;
                avgDist += d;
                neighbours++;
            }
        }

        if (neighbours > 0) {
            avgVel /= static_cast<float>(neighbours);
            avgVel = normalise(avgVel);
            avgDist /= static_cast<float>(neighbours);

            totalCohesion += avgDist;
            totalAlignment += dot(normalise(b.velocity), avgVel);
            count++;
        }
    }

    if (count > 0) {
        metrics.averageCohesion = totalCohesion / static_cast<float>(count);
        metrics.averageAlignment = totalAlignment / static_cast<float>(count);
    }

    return metrics;
}
