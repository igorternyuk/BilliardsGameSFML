#pragma once
#include <random>

class Random
{
public:
    explicit Random();
    int nextInt(int max);
    int nextInt(int min, int max);
    double nextDouble(double max);
    double nextDouble(double min, double max);
    bool nextBool();
private:
    std::random_device m_randomDevice;
    std::mt19937 m_mt{m_randomDevice()};
};
