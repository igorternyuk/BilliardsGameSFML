#pragma once

#include <cmath>

namespace billards
{
    class Pocket
    {
    public:
        explicit Pocket(double cx, double cy, double a, double b);
        bool isBallInside(double bx, double by) const;
    private:
        double center_x_, center_y_;
        double semimajorAxis_, semiminorAxis_;
    };
}


