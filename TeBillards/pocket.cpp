#include "pocket.h"

billards::Pocket::Pocket(double cx, double cy, double a, double b) :
    center_x_(cx), center_y_(cy), semimajorAxis_(a), semiminorAxis_(b)
{}

bool billards::Pocket::isBallInside(double bx, double by) const
{
    return pow((bx - center_x_) / semimajorAxis_, 2) +
           pow((by - center_y_) / semiminorAxis_, 2) < 1;
}
