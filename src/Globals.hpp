#pragma once

#include "AIBoidConfig.hpp"
#include "FitnessConfig.hpp"

constexpr int g_WIDTH = 1920;
constexpr int g_HEIGHT = 1080;
constexpr float PI = 3.14159265f;
constexpr float TWO_PI = 6.28318530f;

extern bool g_drawFOV;
extern FitnessConfig g_fitnessConfig;
extern AIBoidConfig g_aiBoidConfig;