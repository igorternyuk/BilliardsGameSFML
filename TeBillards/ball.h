#pragma once

#include "utils.h"
#include "constants.h"

#include <cmath>
#include <iostream>

//#define DEBUG_BALL

namespace billards {
    class Ball
    {
    public:
        explicit Ball(int index, float vx, float vy, double radius, double x, double y,
                      bool isCueBall, const sf::Color &color);
        inline auto x() const { return circle_.getPosition().x; }
        inline auto y() const { return circle_.getPosition().y; }
        inline auto vx() const { return velocity_.x; }
        inline auto vy() const { return velocity_.y; }
        inline auto v() const { return sqrt(vx() * vx() + vy() * vy()); }
        inline auto angle() const { return atan2(vy(), vx()); }
        inline auto radius() const { return circle_.getRadius(); }
        inline auto top() const { return y() - radius(); }
        inline auto bottom() const { return y() + radius(); }
        inline auto left() const { return x() - radius(); }
        inline auto right() const { return x() + radius(); }

        inline auto isInPlay() const { return isInPlay_; }
        inline void putBallToThePocket() { isInPlay_ = false; }
        inline void activate() {isInPlay_ = true;}
        inline auto isCueBall() const { return isCueBall_; }
        inline auto getColor() const { return circle_.getFillColor(); }
        inline void repaintAsCueBall() { circle_.setFillColor(billards::constants::orangeColor);}
        void selectAsCueBall();
        bool isCursorInside(int cursor_x, int cursor_y) const;
        bool move(float t);
        void move(float dx, float dy);
        void push(double vx, double vy);
        void stop();
        void reflectFromVerticalWall();
        void reflectFromHorizontalWall();
        void reflectFromAnglePlane(double alpha_rad);
        void draw(sf::RenderWindow &window) const { if(isInPlay_) window.draw(circle_);}
    #ifdef DEBUG_BALL
        void printInfo() const;
    #endif
        int getIndex() const;

    private:
        int index_;
        sf::CircleShape circle_;
        sf::Vector2f velocity_;
        bool isCueBall_{false};
        bool isInPlay_{true};
        double impactVelocity_{0.f};

        void decelerate();
        bool handleWallBounce();
        void checkIfOutside();
    };
}

