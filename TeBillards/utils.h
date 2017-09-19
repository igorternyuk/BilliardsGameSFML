#pragma once

#include "SFML/Graphics.hpp"

namespace billards
{
    namespace MathFunctions
    {
        void straightLineCoef(const sf::Vector2f &point1,
                              const sf::Vector2f &point2,
                              double &k, double &b, double &alpha_rad);
        double distanceFromPointToLine(double k, double b, double x, double y);
        bool crossPointOfCircleAndLine(const sf::Vector2f &circle_center,
                                       double radius, double k,
                                       double b, sf::Vector2f &crossPoint1,
                                       sf::Vector2f &crossPoint2);
        bool squareEquation(double A, double B, double C, double &x1,
                            double &x2);
        double distanceBetweenTwoPoints(const sf::Vector2f &point1,
                                        const sf::Vector2f &point2);
        double distanceBetweenTwoPoints(double x1, double y1,
                                        double x2, double y2);
        bool isPointInsideOfTheEllipse(double x, double y,
                                       double ellipse_cx, double ellipse_cy,
                                       double ellipse_a, double ellipse_b);
    }
}

