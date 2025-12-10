#include "NeuralNetwork.hpp"

#include <chrono>
#include <cmath>
#include <random>

static std::mt19937& rng() {
    static std::mt19937 mt(
        (unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());

    return mt;
}

float NeuralNetwork::randFloat(float a, float b) {
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng());
}

float NeuralNetwork::activation(float x) {
    return std::tanh(x);
}

NeuralNetwork::NeuralNetwork(const std::vector<int>& layers) {
    NeuralNetwork::layers = layers;

    if (NeuralNetwork::layers.size() < 2) return;

    NeuralNetwork::weights.resize(NeuralNetwork::layers.size() - 1);
    NeuralNetwork::biases.resize(NeuralNetwork::layers.size() - 1);

    for (size_t l = 0; l + 1 < NeuralNetwork::layers.size(); ++l) {
        int in = NeuralNetwork::layers[l];
        int out = NeuralNetwork::layers[l + 1];

        NeuralNetwork::weights[l].resize(out * in);
        NeuralNetwork::biases[l].resize(out);
    }
}

std::vector<float> NeuralNetwork::feedForward(const std::vector<float>& inputs) const {
    if (NeuralNetwork::layers.empty()) return {};

    std::vector<float> a = inputs;
    // propogate
    for (size_t l = 0; l + 1 < NeuralNetwork::layers.size(); ++l) {
        int in = NeuralNetwork::layers[l];
        int out = NeuralNetwork::layers[l + 1];
        std::vector<float> z(out, 0.f);

        const auto& W = NeuralNetwork::weights[l];
        const auto& B = NeuralNetwork::biases[l];

        for (int i = 0; i < out; ++i) {
            float sum = B[i];
            // W row * a
            int rowStart = i * in;
            for (int j = 0; j < in; ++j) {
                sum += W[rowStart + j] * a[j];
            }
            z[i] = activation(sum);
        }
        a = std::move(z);
    }
    return a;
}

void NeuralNetwork::mutate(float rate, float chance) {
    std::normal_distribution<float> gauss(0.f, rate);
    std::uniform_real_distribution<float> coin(0.f, 1.f);

    for (auto& mat : NeuralNetwork::weights) {
        for (auto& w : mat) {
            if (coin(rng()) <= chance) {
                w += gauss(rng());
            }
        }
    }
    for (auto& bvec : NeuralNetwork::biases) {
        for (auto& b : bvec) {
            if (coin(rng()) <= chance) {
                b += gauss(rng());
            }
        }
    }
}