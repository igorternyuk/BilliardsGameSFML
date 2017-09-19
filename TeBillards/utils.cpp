#include "utils.h"

#include <cmath>

void billards::MathFunctions::straightLineCoef(const sf::Vector2f &point1,
                                     const sf::Vector2f &point2,
                                     double &k, double &b, double &alpha_rad)
{
    k = (point2.y - point1.y) / (point2.x - point1.x);
    b = point1.y - k * point1.x;
    alpha_rad = atan2(point2.y - point1.y, point2.x - point1.x);
}

double billards::MathFunctions::distanceFromPointToLine(double k, double b,
                                                        double x, double y)
{
   double chisl = fabs(k * x - y + b);
   double znam = sqrt(1 + k * k);
   return chisl / znam;
}

bool billards::MathFunctions::crossPointOfCircleAndLine(const sf::Vector2f &circle_center,
                                              double radius,
                                              double k,
                                              double b,
                                              sf::Vector2f &crossPoint1,
                                              sf::Vector2f &crossPoint2)
{
    //Coefficients of the square equation
    double A {k * k + 1};
    double B {2 * (k * (b - circle_center.y) - circle_center.x)};
    double C {circle_center.x * circle_center.x + circle_center.y *
              circle_center.y - radius * radius + b * b - 2 *
              circle_center.y * b};
    double x1{0}, x2{0};
    bool hasCrossPoints{false};
    if(squareEquation(A, B, C, x1, x2))
    {
        crossPoint1.x = x1;
        crossPoint1.y = k * x1 + b;
        crossPoint2.x = x2;
        crossPoint2.y = k * x2 + b;
        hasCrossPoints = true;
    }
    return hasCrossPoints;
}

bool billards::MathFunctions::squareEquation(double A, double B,
                                   double C, double &x1, double &x2)
{
    bool hasRealRoots;
    double D = B * B - 4 * A * C;
    if(D >= 0)
    {
        x1 = (-B + sqrt(D)) / 2 / A;
        x2 = (-B - sqrt(D)) / 2 / A;
        hasRealRoots = true;
    }
    else
    {
        x1 = 0;
        x2 = 0;
        hasRealRoots = false;
    }
    return hasRealRoots;
}

double billards::MathFunctions::distanceBetweenTwoPoints(
        const sf::Vector2f &point1, const sf::Vector2f &point2)
{
    return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
}

double billards::MathFunctions::distanceBetweenTwoPoints(double x1, double y1,
                                                         double x2, double y2)
{
   return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

bool billards::MathFunctions::  isPointInsideOfTheEllipse(double x, double y,
                                                          double ellipse_cx,
                                                          double ellipse_cy,
                                                          double ellipse_a,
                                                          double ellipse_b)
{
    return pow((x - ellipse_cx) / ellipse_a, 2) + pow((y - ellipse_cy) / ellipse_b, 2) < 1;
}
