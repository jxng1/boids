#include "AIBoid.hpp"

#include <algorithm>
#include <cmath>

#include "Utils.hpp"

AIBoid::AIBoid(float x, float y, NeuralNetwork brainTemplate)
    : Boid(x, y), brain(std::move(brainTemplate)) {
}

float AIBoid::computeFitness() const {
    const FitnessConfig cfg = g_fitnessConfig;

    float survivalComponent = survivalTime;
    if (cfg.normaliseSurvival && cfg.maxSurvivalTime > 0.f) {
        survivalComponent = survivalTime / cfg.maxSurvivalTime;
    }

    if (cfg.logSurvival) {
        survivalComponent = std::log(1.f + survivalTime);
    }

    return (survivalComponent * cfg.survivalWeight) + (cohesionScore * cfg.cohesionWeight) +
           (alignmentScore * cfg.alignmentWeight) + (perceptionScore * cfg.perceptionWeight) -
           (dangerScore * cfg.dangerWeight);
}

bool AIBoid::inFOV(const sf::Vector2f& target, float maxDist, float coneDegrees) const {
    sf::Vector2f to = target - position;
    float dist = mag(to);
    if (dist > maxDist) return false;

    float heading = std::atan2(velocity.y, velocity.x);
    float ang = std::atan2(to.y, to.x);

    float diff = ang - heading;
    while (diff > PI) diff -= TWO_PI;
    while (diff < -PI) diff += TWO_PI;

    float half = coneDegrees * 0.5f * PI / 180.f;
    return std::abs(diff) < half;
}

void AIBoid::senseNeighbours(const std::vector<Boid>& boids, float& countNorm, sf::Vector2f& avgDir,
                             sf::Vector2f& avgVel, float& distNorm) {
    int count = 0;
    sf::Vector2f sumDir(0.f, 0.f);
    sf::Vector2f sumVel(0.f, 0.f);
    float totalDist = 0.f;

    float collisionThreshold = 0.1f * fovRadius;

    for (const Boid& other : boids) {
        if (&other == this) continue;

        if (!inFOV(other.position, fovRadius, fovDeg)) continue;

        sf::Vector2f d = other.position - position;
        float dist = mag(d);

        sumDir += d;
        sumVel += other.velocity;
        totalDist += dist;
        count++;

        // penalise boids "colliding" into one another based on distance
        if (dist < collisionThreshold) {
            dangerScore += (collisionThreshold - dist) / collisionThreshold;  // [0, 1]
        }
    }

    if (count > 0) {
        avgDir = sumDir / (float)count;
        avgVel = sumVel / (float)count;
        distNorm = std::min(1.f, (totalDist / count) / fovRadius);
    } else {
        avgDir = {0.f, 0.f};
        avgVel = {0.f, 0.f};
        distNorm = 1.f;

        return;
    }

    countNorm = std::min(1.f, count / 10.f);  // normalised
    // perception
    perceptionScore += countNorm;

    // cohesion
    float cohesion = countNorm * (1.f - distNorm);
    cohesionScore += cohesion;

    float alignment = 0.f;
    if (mag(avgVel) > 0.0001f && mag(velocity) > 0.0001f) {
        sf::Vector2 nv = normalise(velocity);
        sf::Vector2 nav = normalise(avgVel);
        float dotv = dot(nv, nav);        // [-1, 1]
        alignment = (dotv + 1.f) * 0.5f;  // [0, 1]
    }
    alignmentScore += alignment;
}

void AIBoid::sensePredators(const std::vector<Predator>& preds, float& predDistNorm,
                            float& predRelAngle) {
    float closest = 1e9f;
    sf::Vector2f closestVec(0.f, 0.f);
    bool found = false;

    for (const Predator& p : preds) {
        if (!inFOV(p.position, predatorFOVRadius, predatorFOVDeg)) continue;

        sf::Vector2f d = p.position - position;
        float dist = mag(d);
        if (dist < closest) {
            closest = dist;
            closestVec = d;
            found = true;
        }
    }

    if (!found) {
        predDistNorm = 1.f;
        predRelAngle = 0.f;
        dangerScore = 0.f;
        return;
    }

    // normalised distance to predator: e.g. 1 when far, 0 when on top
    predDistNorm = std::min(1.f, closest / predatorFOVRadius);

    // angle relative to heading
    float heading = std::atan2(velocity.y, velocity.x);
    float ang = std::atan2(closestVec.y, closestVec.x);
    float rel = ang - heading;

    while (rel > PI) rel -= TWO_PI;
    while (rel < PI) rel += TWO_PI;

    predRelAngle = rel / PI;  // normalise to [-1, 1]

    // danger score
    float distDanger = 1.f - predDistNorm;              // [0(far), 1(close)]
    float angleDanger = 1.f - std::fabs(predRelAngle);  // [0(behind), 1(in front)]
    dangerScore = g_fitnessConfig.dangerDistWeight * distDanger +
                  g_fitnessConfig.dangerAngleWeight * angleDanger;

    // clamp
    if (dangerScore < 0.f) dangerScore = 0.f;
    if (dangerScore > 1.f) dangerScore = 1.f;
}

float AIBoid::angleBetweenRadians(const sf::Vector2f& a, const sf::Vector2f& b) const {
    float al = mag(a);
    float bl = mag(b);
    if (al < 1e-6f || bl < 1e-6f) return 0.f;
    float dot = (a.x * b.x + a.y * b.y) / (al * bl);
    dot = std::max(-1.0f, std::min(1.0f, dot));
    return std::acos(dot);
}

void AIBoid::think(const std::vector<Boid>& boids, const std::vector<Predator>& preds, float dt) {
    float countNorm, distNorm;
    sf::Vector2f avgDir, avgVel;
    senseNeighbours(boids, countNorm, avgDir, avgVel, distNorm);

    float predDistNorm, predRelAngle;
    sensePredators(preds, predDistNorm, predRelAngle);

    survivalTime += dt;

    avgDir = normalise(avgDir);
    avgVel = normalise(avgVel);

    // heading
    sf::Vector2f forward = normalise(velocity);
    if (mag(velocity) < 1e-5f) forward = {1.f, 0.f};
    sf::Vector2f right = {forward.y, -forward.x};
    // transform to local frame
    float localDirForward = avgDir.x * forward.x + avgDir.y * forward.y;  // dir forward
    float localDirRight = avgDir.x * right.x + avgDir.y * right.y;        // dir right
    float localVelForward = avgVel.x * forward.x + avgVel.y * forward.y;  // vel forward
    float localVelRight = avgVel.x * right.x + avgVel.y * right.y;        // vel right

    float speedNorm = mag(velocity) / maxSpeed;

    // ------ Neural Network Inputs ------
    std::vector<float> inputs = {
        countNorm,        // 0: countNorm
        localDirForward,  // 1: localDirForward
        localDirRight,    // 2: localDirRight
        localVelForward,  // 3: localVelForward
        localVelRight,    // 4: localVelRight
        distNorm,         // 5: distNorm
        speedNorm,        // 6: speedNorm
        predDistNorm,     // 7: predDistNorm
        predRelAngle,     // 8: predRelAngle
        1.f               // 9 bias
    };
    auto out = brain.feedForward(inputs);

    // Normalise steer
    sf::Vector2f steer(out[0], out[1]);
    if (mag(steer) > 1.f) steer = normalise(steer);

    Boid::applyForce(steer * maxForce);

    // Clamp rotation to stop 360 spins
    float speed = mag(velocity);
    if (speed > 1e-5f) {
        float currentAngle = std::atan2(forward.y, forward.x);
        float newAngle = std::atan2(velocity.y, velocity.x);

        float delta = newAngle - currentAngle;

        // wrap to [-pi, pi]
        while (delta > PI) delta -= TWO_PI;
        while (delta < -PI) delta += TWO_PI;

        float maxTurnRad = g_aiBoidConfig.maxTurnDeg * PI / 180.f * dt;
        delta = std::clamp(delta, -maxTurnRad, maxTurnRad);

        float clampedAngle = currentAngle + delta;
        velocity.x = std::cos(clampedAngle) * speed;
        velocity.y = std::sin(clampedAngle) * speed;
    }
}

void AIBoid::draw(sf::RenderWindow& window) {
    // triangle body
    sf::ConvexShape tri;
    tri.setPointCount(3);
    tri.setPoint(0, sf::Vector2f(0.f, -6.f));
    tri.setPoint(1, sf::Vector2f(-4.f, 6.f));
    tri.setPoint(2, sf::Vector2f(4.f, 6.f));

    float angle = std::atan2(velocity.y, velocity.x) * 180.f / PI + 90.f;

    tri.setPosition(position);
    tri.setRotation(sf::degrees(angle));
    tri.setFillColor(sf::Color::White);
    window.draw(tri);

    // FOV debug?
    if (!g_drawFOV) return;

    sf::ConvexShape cone;
    const int seg = 24;
    cone.setPointCount(seg + 2);
    cone.setPoint(0, {0, 0});

    float heading = std::atan2(velocity.y, velocity.x);

    for (int i = 0; i <= seg; i++) {
        float t = -fovDeg / 2 + (fovDeg * i / seg);
        float ang = heading + t * PI / 180.f;
        cone.setPoint(i + 1, {std::cos(ang) * fovRadius, std::sin(ang) * fovRadius});
    }

    cone.setFillColor(sf::Color(255, 255, 255, 40));
    cone.setPosition(position);
    window.draw(cone);
}