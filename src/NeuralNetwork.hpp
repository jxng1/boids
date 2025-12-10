#pragma once
#include <random>
#include <vector>

class NeuralNetwork {
   public:
    NeuralNetwork() = default;
    NeuralNetwork(const std::vector<int>& layers);

    std::vector<float> feedForward(const std::vector<float>& inputs) const;

    void mutate(float rate, float chance = 1.f);

    NeuralNetwork clone() const {
        return *this;
    }

   private:
    std::vector<int> layers;
    std::vector<std::vector<float>> weights;
    std::vector<std::vector<float>> biases;

    static float activation(float x);
    static float randFloat(float a, float b);
};