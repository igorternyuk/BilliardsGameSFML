#pragma once

#include "SFML/Graphics.hpp"

#include <vector>
#include <string>

namespace billards
{
    namespace constants
    {
        const std::string titleOfProgram{"TeBillards"};
        const std::string winMessage1{"PLAYER 1 WON!!!"};
        const std::string winMessage2{"PLAYER 2 WON!!!"};
        const std::string help
        {
            "A- free pyramid, M- russian pyramid,\n"
            "P- pool R- setup ball randomly,\n"
            "wheel - adjust power,\n"
            "first left click - select the ball,\n"
            "second left click - push the ball,\n"
            "right click- cancel selection, +/- adjust volume,\n"
            "S - back to menu, Esc- quit\n"
        };
        const std::vector<std::string> gameMenu
        {
            {"1.Free pyramid (press A)."},
            {"2.Russian pyramid (press M)."},
            {"3.Pool8 (Press P)."},
            {"4.Random setup (Press R)."},
            {"5.Quit game (Press Q)."}
        };
        constexpr int windowWidth{915}, windowHeight{670};
        constexpr int fieldWidth{838}, fieldHeight{422};
        constexpr int fieldLeft{40}, fieldTop{40}, fieldRight{878}, fieldBottom{462};
        constexpr int ballCount{16}, pocketCount{6};
        constexpr double ballRadius{10.f}, minBallVelocity{1.0f};
        constexpr double frictionFactor{0.9f};
        const std::vector<sf::Vector2f> ballsArrangement
        {
            {228, 250}, {686, 250}, {704, 239}, {704, 261},
            {723, 229}, {723, 251}, {723, 273}, {741, 216},
            {741, 239}, {741, 260}, {741, 283}, {759, 206},
            {759, 227}, {759, 251}, {759, 274}, {759, 296}
        };
        const std::vector<std::vector<double>> pocketsData1
        {
            {28, 28, 24, 24},
            {28, 471, 24, 24},
            {459, 23, 29, 14},
            {459, 477, 29, 14},
            {888, 28, 24, 24},
            {888, 471, 24, 24}
        };
        const std::vector<std::vector<std::vector<double>>> pocketsData2
        {
        {{70, 70},{1.2, 23.8, 0.9, -22.5}},
        {{70, 431},{-1.114, 473, -0.797, 517}},
        {{435, 481, 35},{-8, 3535, 8, -3809}},
        {{435, 481, 465},{5.7, -2021.9, -5.27, 2994}},
        {{850, 65},{-0.76, 682.6, -1.313, 1224}},
        {{850, 434},{0.896, -300.624, 1.2, -625}}
        };
        const std::vector<std::pair<std::string, std::string>> gameSoundsPathes
        {
            {"ball_ball", "Resources/Sounds/ballball.wav"},
            {"ball_wall","Resources/Sounds/board01.wav"},
            {"ball_pocket","Resources/Sounds/pocket1.wav"}
        };

        const std::vector<std::string> ballSoundsPathes
        {
            "Resources/Sounds/ball.wav",
            "Resources/Sounds/ball01s.wav",
            "Resources/Sounds/ball02s.wav",
            "Resources/Sounds/ball03s.wav",
            "Resources/Sounds/cue05.wav",
            "Resources/Sounds/cue10.wav",
            "Resources/Sounds/cuehit.wav"
        };
        const sf::Color orangeColor = sf::Color(255, 174, 0);
        const sf::Color darkRedColor = sf::Color(127, 0, 0);
        const int winBallCount = 7;
    }
}


