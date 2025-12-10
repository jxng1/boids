#pragma once

struct AIBoidConfig {
    float perceptionRadius = 50.f;

    // fov
    float fovDeg = 120.f;
    float fovRadius = 120.f;

    // pred fov
    float predatorFOVDeg = 160.f;
    float predatorFOVRadius = 250.f;

    // motion
    float maxTurnDeg = 120.f;
    float maxAcceleration = 60.f;
    float minSpeed = 10.f;
    float maxSpeedAI = 200.f;
};