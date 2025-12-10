#pragma once
#include <functional>
#include <vector>

#include "AIBoid.hpp"

struct EvolutionConfig {
    int population = 100;
    int elites = 10;   // keep top N without mutation
    int parents = 30;  // number selected as parents
    float mutationRate = 0.2f;
    float mutationChance = 0.3f;
    float generationTime = 20.f;  // seconds per simulation
};

class EvolutionManager {
   public:
    EvolutionManager(const EvolutionConfig& cfg, const NeuralNetwork& templateNet);

    // initialise random population
    void initialise();

    // run one generation (simulate externally - manager keeps track of time)
    void update(float dt);

    // called by simulation when boid dies or at generation end to set fitness
    void setFitness(int index, float fitness);

    // after generationTime passes, evolve -> create next gen
    bool generationComplete() const;

    // produce/reset boids array (call from main)
    std::vector<AIBoid> producePopulation();

    int getGeneration() const {
        return m_generation;
    }

   private:
    EvolutionConfig cfg;
    std::vector<NeuralNetwork> m_pool;  // neural networks for population
    std::vector<float> m_fitness;       // fitness per individual
    float m_time = 0.f;
    int m_generation = 0;
    bool m_justEvolved = false;

    // internal helpers
    void evolve();
    int tournamentSelect() const;
};
