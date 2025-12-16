#pragma once
#include "Boid.hpp"
#include "Globals.hpp"
#include "NeuralNetwork.hpp"
#include "Predator.hpp"
#include "SFML/System/Angle.hpp"

class AIBoid : public Boid {
   public:
    AIBoid(float x, float y, NeuralNetwork brainTemplate = NeuralNetwork({10, 12, 2}));

    void think(const std::vector<Boid>& boids, const std::vector<Predator>& predators, float dt);

    void draw(sf::RenderWindow& window);

    // fov
    float fovDeg = g_aiBoidConfig.fovDeg;
    float fovRadius = g_aiBoidConfig.fovRadius;

    // pred fov
    float predatorFOVDeg = g_aiBoidConfig.predatorFOVDeg;
    float predatorFOVRadius = g_aiBoidConfig.predatorFOVRadius;

    // motion
    float maxTurnDeg = g_aiBoidConfig.maxTurnDeg;
    float maxAcceleration = g_aiBoidConfig.maxAcceleration;
    float minSpeed = g_aiBoidConfig.minSpeed;
    float maxSpeedAI = g_aiBoidConfig.maxSpeedAI;

    // fitness
    float survivalTime = 0.f;
    float cohesionScore = 0.f;
    float cohesionWeight = 0.f;
    float alignmentScore = 0.f;
    float alignmentWeight = 0.f;
    float perceptionScore = 0.f;
    float perceptionWeight = 0.f;
    float dangerScore = 0.f;
    float dangerWeight = 0.f;

    void mutateBrain(float rate = 0.1f, float chance = 0.5f) {
        brain.mutate(rate, chance);
    }
    float computeFitness() const;

   private:
    NeuralNetwork brain;

    // returns: count, avgDir, avgVel, avgDist
    void senseNeighbours(const std::vector<Boid>& boids, float& countNorm, sf::Vector2f& avgDir,
                         sf::Vector2f& avgVel, float& distNorm);

    // returns predator distance normalised + relative angle (-1..1)
    void sensePredators(const std::vector<Predator>& preds, float& predDistNorm,
                        float& predRelAngle);

    float angleBetweenRadians(const sf::Vector2f& a, const sf::Vector2f& b) const;
    bool inFOV(const sf::Vector2f& target, float maxDist, float coneDegrees) const;
};
