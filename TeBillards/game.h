#pragma once

#include <SFML/Audio.hpp>
#include "utils.h"
#include "constants.h"
#include "ball.h"
#include "pocket.h"
#include "random.h"

#include <map>
#include <memory>

namespace billards
{
    class Game
    {
    public:
        explicit Game();
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
        Game(Game&&) = delete;
        Game& operator=(Game&&) = delete;
        void run();
    private:
        enum
        {
            FPS = 60,
            INSTRUCTIONS_X = 465,
            INSTRUCTIONS_Y = 540,
            DEFAULT_VOLUME = 50,
            DEFAUT_IMPACT_FORCE = 100,
            GAME_TITLE_FONT_SIZE = 65,
            MENU_TITLE_FONT_SIZE = 70,
            MENU_ITEMS_FONT_SIZE = 30,
            GAME_INSTRUCTIONS_FONT_SIZE = 14
        };
        enum class GameState
        {
            PLAYER1_TO_PLAY,
            PLAYER2_TO_PLAY,
            PLAYER1_WON,
            PLAYER2_WON
        };
        enum class GameType {
            AMERIKANKA,
            PIRAMIDA,
            POOL,
            RANDOM
        };
        mutable sf::RenderWindow window_;
        sf::RectangleShape fieldRect_;
        std::vector<std::unique_ptr<Ball>> balls_;
        std::vector<std::unique_ptr<Pocket>> pockets_;
        GameState gameState_{GameState::PLAYER1_TO_PLAY};
        GameType gameType_{GameType::PIRAMIDA};
        bool isLegalHit_{false};
        bool isBallChoosen_{false};
        bool isGameTypeSwitched_{false};
        bool isMenuActive_{true};
        int selectedMenuItem_{0};
        unsigned int choosenBallIndex_{0};
        double impactForce_{DEFAUT_IMPACT_FORCE};
        unsigned int cueBallIndex_{0};
        int firstPlayerScore_{0};
        int secondPlayerScore_{0};
        int firstPlayerScoreOld_ {firstPlayerScore_};
        int secondPlayerScoreOld_ {secondPlayerScore_};
        bool isTurnChecked_{true};
        mutable sf::Vector2f currCursorPos_;
        Random random_;
        sf::Font font_;
        sf::Texture tableTexture_;
        sf::Sprite tableSprite_;
        std::vector<sf::SoundBuffer> cueHitsSoundBuffers_;
        std::vector<sf::Sound> cueHits_;
        std::vector<sf::SoundBuffer> gameSoundsBuffers_;
        std::map<std::string, sf::Sound> sounds_;
        sf::Music background_;
        double volume_{DEFAULT_VOLUME};
        sf::Text menuTitle_{"TeBillards", font_, GAME_TITLE_FONT_SIZE};
        sf::Text menuWord_{"MENU", font_, MENU_TITLE_FONT_SIZE};
        sf::Text menuHint1_{"Use arrow key for menu navigation", font_,
                    MENU_ITEMS_FONT_SIZE};
        sf::Text menuHint2_{"Select game mode by pressing ENTER", font_,
                    MENU_ITEMS_FONT_SIZE};
        sf::Text txtInstructions_{billards::constants::help, font_,
                    GAME_INSTRUCTIONS_FONT_SIZE};
        mutable sf::Text firstPlayerName_{sf::String("Player1:"), font_, 30};
        mutable sf::Text secondPlayerName_{sf::String("Player2:"), font_, 30};
        mutable sf::Text status_{sf::String(""), font_, 40};
        mutable sf::Text txtForceAndVolume_{sf::String(""), font_, 23};
        mutable std::vector<sf::Text> menuItems_;

        void inputPhaseMenu();
        void inputPhasePlay();
        void updatePhase();
        void drawPhasePlay();
        void createBalls(GameType gameType);
        void prepareNewGame(GameType gameType);
        void generateRandomBallSetup();
        void createPockets();
        void loadSprites();
        void loadFonts();
        void loadSounds();
        void playRandCueHitSound();
        bool areAllBallsStopped();
        bool checkCollision(double b1_x, double b1_y, double b1_r, double b2_x,
                            double b2_y, double b2_r);
        bool checkCollision(Ball &ball1, Ball &ball2);
        bool checkCollision(Ball &ball1, Ball &ball2, double &overlapX,
                            double &overlapY, double &angle_rad);
        bool resolveCollision(Ball &ball1, Ball &ball2);
        bool hasFirstPlayerBalls();
        bool hasSecondPlayerBalls();
        bool checkPockettHit(Ball &ball);
        bool setupBallRandomlyInTheHouseArea(unsigned int index);
        void configureTextLabels();
        void menuItemActivate(int item);
        void drawMenu() const;
        void drawGameInfo() const;
        void drawPlayerNames() const;
        void drawPlayersScore() const;
        void drawGameType() const;
        void drawForceAndVolume() const;
        void updateWindowTitle() const;
        void drawInstructions() const;
        void drawField() const;
        void drawHint() const;
        void drawWinMessage() const;
        void calcHintParam(sf::Vector2f &p1, sf::Vector2f &p2, sf::Vector2f &p3,
                           sf::Vector2f &p4, bool &isBallsOnTheWay) const;
    };
}

