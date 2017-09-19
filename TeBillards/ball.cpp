#include "ball.h"

#ifdef DEBUG_BALL
#include <iostream>
#endif

billards::Ball::Ball(int index, float vx, float vy, double radius, double x, double y,
           bool isCueBall, const sf::Color &color) :
    velocity_({vx, vy}), isCueBall_(isCueBall)
{
    index_ = index;
    circle_.setRadius(radius);
    circle_.setOrigin(radius, radius);
    circle_.setPosition(x, y);
    isCueBall_ = isCueBall;
    circle_.setFillColor(color);
}

void billards::Ball::selectAsCueBall()
{
    isCueBall_ = true;
    circle_.setFillColor(billards::constants::orangeColor);
}

bool billards::Ball::isCursorInside(int cursor_x, int cursor_y) const
{
    return pow(cursor_x - x(), 2) + pow(cursor_y - y(), 2) < radius() * radius();
}

bool billards::Ball::move(float t)
{
    bool result{handleWallBounce()};
    circle_.move({velocity_.x * t, velocity_.y * t});
   //Moving ball will decelerate by friction force
    decelerate();
    checkIfOutside();
    return result;
}

void billards::Ball::move(float dx, float dy)
{
    circle_.move({dx, dy});
    checkIfOutside();
}

void billards::Ball::push(double vx, double vy)
{
    velocity_.x = vx;
    velocity_.y = vy;
}

void billards::Ball::stop()
{
    velocity_.x = 0.f;
    velocity_.y = 0.f;
}

void billards::Ball::reflectFromVerticalWall()
{
    velocity_.x = -velocity_.x;
}

void billards::Ball::reflectFromHorizontalWall()
{
    velocity_.y = -velocity_.y;
}

void billards::Ball::reflectFromAnglePlane(double alpha_rad)
{
#ifdef DEBUG_BALL
    std::cout << "Reflection from diagonal wall of the pocket" << std::endl;
    std::cout << "velocity_.x = " << velocity_.x << std::endl;
    std::cout << "velocity_.y = " << velocity_.y << std::endl;
    std::cout << "alpha_rad = " << alpha_rad << std::endl;
    std::cout << "angle()= " << angle() << std::endl;
#endif
    //Here we rotate the coordinate system which has origin in the ball and wall contact point
    double vx_new_rotated = vx() * cos(alpha_rad) + vy() * sin(alpha_rad);
    double vy_new_rotated = -vx() * sin(alpha_rad) + vy() * cos(alpha_rad);
    vy_new_rotated = -vy_new_rotated; //Reflection of the ball
    //Rotate back the coordinate system
    velocity_.x = vx_new_rotated * cos(alpha_rad) - vy_new_rotated * sin(alpha_rad);
    velocity_.y = vx_new_rotated * sin(alpha_rad) + vy_new_rotated * cos(alpha_rad);
#ifdef DEBUG_BALL
    std::cout << "Ball velocity components after reflection" << std::endl;
    std::cout << "velocity_.x = " << velocity_.x << std::endl;
    std::cout << "velocity_.y = " << velocity_.y << std::endl;
    std::cout << "angle()= " << angle() << std::endl;
    std::cout << "Ball coordinates before desplacement:" << std::endl;
    std::cout << "x() = " << x() << " y() = " << y() << std::endl;
#endif
    this->move(vx() / fabs(vx()) * (radius() + 1) , vy() / fabs(vy()) * (radius() + 1));
#ifdef DEBUG_BALL
    std::cout << "Ball coordinates after desplacement:"<< std::endl;
    std::cout << "x() = " << x() << " y() = " << y() << std::endl;
#endif
}

#ifdef DEBUG_BALL
void billards::Ball::printInfo() const //For debugging only
{
    std::cout << "x = " << x() << " y = " << y() << std::endl;
    std::cout << "left = " << left() << " right = " << right() << std::endl;
    std::cout << "top = " << top() << " bottom = " << bottom() << std::endl;
    std::cout << "vx = " << vx() << " vy = " << vy() << " v = " << v() << std::endl;
    std::cout << "alpha = " << angle() / M_PI * 180 << std::endl;
}
#endif

int billards::Ball::getIndex() const
{
    return index_;
}

void billards::Ball::decelerate()
{
    static int tick = 0;
    if(++tick > 30)
    {
        tick = 0;
        velocity_.x *= billards::constants::frictionFactor;
        velocity_.y *= billards::constants::frictionFactor;
        if(velocity_.x > -billards::constants::minBallVelocity && velocity_.x < +billards::constants::minBallVelocity)
            velocity_.x = 0.f;
        if(velocity_.y > -billards::constants::minBallVelocity && velocity_.y < +billards::constants::minBallVelocity)
            velocity_.y = 0.f;
    }
}

bool billards::Ball::handleWallBounce()
{
    //Все магические цифры, характеризующие бортики возле входа в лузы
    //заменить на именованные константы
    bool result{false};
    if(x() < billards::constants::pocketsData2[0][0][0] && y() < billards::constants::pocketsData2[0][0][1])
    {
        //Top left pocket
        double d1 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[0][1][0], billards::constants::pocketsData2[0][1][1], x(), y());
        double d2 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[0][1][2], billards::constants::pocketsData2[0][1][3], x(), y());
        if(d1 <= radius())
        {
            reflectFromAnglePlane(atan(billards::constants::pocketsData2[0][1][0]));
            result = true;

        } else if (d2 <= radius())
        {
            reflectFromAnglePlane(atan(billards::constants::pocketsData2[0][1][2]));
            result = true;
        }
    }
    else if(x() < billards::constants::pocketsData2[1][0][0] && y() > billards::constants::pocketsData2[1][0][1])
    {
        //Bottom left pocket
        double d1 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[1][1][0], billards::constants::pocketsData2[1][1][1], x(), y());
        double d2 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[1][1][2], billards::constants::pocketsData2[1][1][3], x(), y());
        if(d1 <= radius())
        {
            reflectFromAnglePlane(billards::constants::pocketsData2[1][1][0]);
            result = true;
        } else if(d2 <= radius())
        {
            reflectFromAnglePlane(billards::constants::pocketsData2[1][1][2]);
            result = true;
        }
    }
    else if(left() > billards::constants::pocketsData2[2][0][0] && right() < billards::constants::pocketsData2[2][0][1])
    {
        //Central top pocket
        if(top() < billards::constants::pocketsData2[2][0][2])
        {
            double d1 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[2][1][0], billards::constants::pocketsData2[2][1][1], x(), y());
            double d2 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[2][1][2], billards::constants::pocketsData2[2][1][3], x(), y());
            if(d1 <= radius())
            {
                reflectFromAnglePlane(billards::constants::pocketsData2[2][1][0]);
                result = true;
            } else if(d2 <= radius())
            {
                reflectFromAnglePlane(billards::constants::pocketsData2[2][1][2]);
                result = true;
            }
        }
    }
    else if(left() > billards::constants::pocketsData2[3][0][0] && right() < billards::constants::pocketsData2[3][0][1])
    {
        //Central bottom pocket
        if(bottom() > billards::constants::pocketsData2[3][0][2])
        {
            double d1 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[3][1][0], billards::constants::pocketsData2[3][1][1], x(), y());
            double d2 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[3][1][2], billards::constants::pocketsData2[3][1][3], x(), y());
            if(d1 < radius())
            {
                 reflectFromAnglePlane(billards::constants::pocketsData2[3][1][0]);
                 result = true;
            } else if(d2 < radius())
            {
                reflectFromAnglePlane(billards::constants::pocketsData2[3][1][2]);
                result = true;
            }
        }
    }
    else if(x() > billards::constants::pocketsData2[4][0][0] && y() < billards::constants::pocketsData2[4][0][1])
    {
        //Top right pocket
        double d1 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[4][1][0], billards::constants::pocketsData2[4][1][1], x(), y());
        double d2 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[4][1][2], billards::constants::pocketsData2[4][1][3], x(), y());
        if(d1 <= radius())
        {
            reflectFromAnglePlane(atan(billards::constants::pocketsData2[4][1][0]));
            result = true;
        } else if(d2 <= radius())
        {
            reflectFromAnglePlane(atan(billards::constants::pocketsData2[4][1][2]));
            result = true;
        }
    }
    else if(x() > billards::constants::pocketsData2[5][0][0] && y() > billards::constants::pocketsData2[5][0][1])
    {
        double d1 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[5][1][0], billards::constants::pocketsData2[5][1][1], x(), y());
        double d2 = billards::MathFunctions::distanceFromPointToLine(billards::constants::pocketsData2[5][1][2], billards::constants::pocketsData2[5][1][3], x(), y());
        if(d1 <= radius())
        {
            reflectFromAnglePlane(atan(billards::constants::pocketsData2[5][1][0]));
            result = true;
        } else if(d2 <= radius())
        {
            reflectFromAnglePlane(atan(billards::constants::pocketsData2[5][1][2]));
            result = true;
        }
    }
    else
    {
        double overLapLeft = left() - billards::constants::fieldLeft;
        double overLapRight = right() - billards::constants::fieldRight;
        if(overLapLeft <= 0)
        {
            move(-overLapLeft, 0);
            reflectFromVerticalWall();
            result = true;
        }
        else if(overLapRight >= 0)
        {
            move(-overLapRight, 0);
            reflectFromVerticalWall();
            result = true;
        }
        double overLapTop = top() - billards::constants::fieldTop;
        double overLapBottom = bottom() - billards::constants::fieldBottom;
        if(overLapTop <= 0)
        {
            move(0, -overLapTop);
            reflectFromHorizontalWall();
            result = true;
        }
        else if(overLapBottom >= 0)
        {
            move(0, -overLapBottom);
            reflectFromHorizontalWall();
            result = true;
        }
    }
    return result;
}

void billards::Ball::checkIfOutside()
{
    if(right() < billards::constants::fieldLeft || left() > billards::constants::fieldRight ||
       top() > billards::constants::fieldBottom || bottom() < billards::constants::fieldTop)
    {
       isInPlay_ = false;
    }
}
