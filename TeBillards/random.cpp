#include "random.h"

Random::Random()
{}

int Random::nextInt(int max)
{
     std::uniform_int_distribution<int> distribution_{0,max};
     return distribution_(m_mt);
}

int Random::nextInt(int min, int max)
{
    std::uniform_int_distribution<int> distribution_{min,max};
    return distribution_(m_mt);
}

double Random::nextDouble(double max)
{
    std::uniform_real_distribution<> distribution_{0,max};
    return distribution_(m_mt);
}

double Random::nextDouble(double min, double max)
{
    std::uniform_real_distribution<> distribution_{min,max};
    return distribution_(m_mt);
}

bool Random::nextBool()
{
    return static_cast<bool>(nextInt(0, 2));
}
