struct FitnessConfig {
    // cohesion weighting on fitness
    float cohesionWeight = 0.5f;

    // alignment weighting on fitness
    float alignmentWeight = 0.7f;

    // perception weighting on fitness
    float perceptionWeight = 0.4f;

    // danger weighting on fitness
    float dangerWeight = 1.0f;
    // danger distance weighting, must add up to 1 for distance and angle
    float dangerDistWeight = 0.8f;
    // danger angle weighting
    float dangerAngleWeight = 0.2f;

    // survival weighting on fitness
    float survivalWeight = 1.0f;

    // optionals
    float maxSurvivalTime = 60.0f;
    bool normaliseSurvival = false;
    bool logSurvival = false;
};
