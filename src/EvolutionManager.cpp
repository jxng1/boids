#include "EvolutionManager.hpp"

#include <algorithm>
#include <chrono>
#include <random>

#include "Globals.hpp"

static std::mt19937& rng() {
    static std::mt19937 mt(
        (unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    return mt;
}

EvolutionManager::EvolutionManager(const EvolutionConfig& c, const NeuralNetwork& templateNet)
    : cfg(c) {
    m_pool.resize(cfg.population, templateNet);
    m_fitness.resize(cfg.population, 0.f);
}

void EvolutionManager::initialise() {
    for (auto& net : m_pool) {
        net.mutate(0.5f, 1.0f);  // randomize initial population
    }
    std::fill(m_fitness.begin(), m_fitness.end(), 0.f);
    m_time = 0.f;
    m_generation = 0;
}

void EvolutionManager::update(float dt) {
    m_justEvolved = false;
    m_time += dt;
    if (m_time >= cfg.generationTime) {
        evolve();
        m_time = 0.f;
        ++m_generation;
        m_justEvolved = true;
    }
}

void EvolutionManager::setFitness(int index, float fitness) {
    if (index >= 0 && index < (int)m_fitness.size()) m_fitness[index] = fitness;
}

bool EvolutionManager::generationComplete() const {
    return m_justEvolved;
}

std::vector<AIBoid> EvolutionManager::producePopulation() {
    std::vector<AIBoid> pop;
    pop.reserve(cfg.population);
    for (int i = 0; i < cfg.population; ++i) {
        // create AIBoid at random positions
        pop.emplace_back((float)(rand() % g_WIDTH), (float)(rand() % g_HEIGHT), m_pool[i]);
    }
    return pop;
}

int EvolutionManager::tournamentSelect() const {
    // simple tournament selection
    std::uniform_int_distribution<int> dist(0, cfg.population - 1);
    int a = dist(rng());
    int b = dist(rng());
    return (m_fitness[a] > m_fitness[b]) ? a : b;
}

void EvolutionManager::evolve() {
    // pair networks with fitness indices
    std::vector<int> idx(cfg.population);
    for (int i = 0; i < cfg.population; ++i) idx[i] = i;

    // sort indices by fitness desc
    std::sort(idx.begin(), idx.end(), [&](int a, int b) { return m_fitness[a] > m_fitness[b]; });

    // create new pool
    std::vector<NeuralNetwork> newPool(cfg.population);

    // elitism: copy top elites unchanged
    for (int i = 0; i < cfg.elites && i < cfg.population; i++) {
        newPool[i] = m_pool[idx[i]];
    }

    // fill rest using tournament selection and mutation
    int cur = cfg.elites;
    std::uniform_real_distribution<float> coin(0.f, 1.f);
    while (cur < cfg.population) {
        int parentIdx = tournamentSelect();
        newPool[cur] = m_pool[parentIdx].clone();
        // mutate child
        newPool[cur].mutate(cfg.mutationRate, cfg.mutationChance);
        ++cur;
    }

    m_pool.swap(newPool);
    std::fill(m_fitness.begin(), m_fitness.end(), 0.f);
}
