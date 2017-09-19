#include "game.h"
#include <sstream>
#ifdef DEBUG
#include <iostream>
#endif

billards::Game::Game():
    window_{sf::VideoMode(constants::windowWidth,
                          constants::windowHeight),
            constants::titleOfProgram.c_str(), sf::Style::Close}

{
    //Centalize window
    window_.setPosition(sf::Vector2i(
                        (sf::VideoMode::getDesktopMode().width -
                         constants::windowWidth) / 2,
                         (sf::VideoMode::getDesktopMode().height -
                         constants::windowHeight) / 2));
    window_.setFramerateLimit(FPS);


    balls_.reserve(constants::ballCount);
    pockets_.reserve(constants::pocketCount);
    createPockets();
#ifdef DEBUG
    std::cout << "pockets_.size = " << pockets_.size() << std::endl;
#endif
    createBalls(GameType::PIRAMIDA);
#ifdef DEBUG
    std::cout << "balls_.size = " << balls_.size() << std::endl;
#endif
    loadSprites();
    loadFonts();
    loadSounds();
    configureTextLabels();
}

void billards::Game::run()
{
    background_.play();
    ////////////Main loop of the game/////////////////
    while(window_.isOpen())
    {
        if(isMenuActive_)
        {
            inputPhaseMenu();
            drawMenu();
        }
        else
        {
            inputPhasePlay();
            updatePhase();
            drawPhasePlay();
        }
        window_.display();
    }    
    background_.stop();
}

void billards::Game::inputPhaseMenu()
{
    sf::Event events;
    while(window_.pollEvent(events))
    {
        if(events.type == sf::Event::Closed)
            window_.close();
        else if(events.type == sf::Event::KeyReleased)
        {
            switch(events.key.code)
            {
                case sf::Keyboard::Up:
                    --selectedMenuItem_;
                    if(selectedMenuItem_ < 0)
                        selectedMenuItem_ = int(constants::
                                                gameMenu.size() - 1);
                    break;
                case sf::Keyboard::Down:
                    ++selectedMenuItem_;
                    if(selectedMenuItem_ > int(constants::
                                               gameMenu.size() - 1))
                        selectedMenuItem_ = 0;
                    break;
                case sf::Keyboard::Return:
                    menuItemActivate(selectedMenuItem_);
                    break;
                default:
                    break;
            }
        }
        else if(events.type == sf::Event::MouseMoved)
        {
            auto cursorX = events.mouseMove.x;
            auto cursorY = events.mouseMove.y;
            for(unsigned int i{0}; i < menuItems_.size(); ++i)
            {
                if(menuItems_.at(i).getGlobalBounds().
                   contains(cursorX, cursorY))
                {
                    selectedMenuItem_ = i;
                    break;
                }
            }
        }
        else if(events.type == sf::Event::MouseButtonReleased)
        {
            int cursorX = events.mouseButton.x, cursorY = events.mouseButton.y;
            if(menuItems_.at(selectedMenuItem_).getGlobalBounds().
                    contains(cursorX, cursorY))
            {
                menuItemActivate(selectedMenuItem_);
            }
        }
    }
}

void billards::Game::inputPhasePlay()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window_.close();

    sf::Event events;
    while(window_.pollEvent(events))
    {
        if(events.type == sf::Event::Closed)
            window_.close();
        else if(events.type == sf::Event::KeyReleased)
        {
            switch(events.key.code)
            {
                case sf::Keyboard::A:
                    prepareNewGame(GameType::AMERIKANKA);
                    isGameTypeSwitched_ = true;
                    break;
                case sf::Keyboard::M:
                    prepareNewGame(GameType::PIRAMIDA);
                    isGameTypeSwitched_ = true;
                    break;
                case sf::Keyboard::P:
                    prepareNewGame(GameType::POOL);
                    isGameTypeSwitched_ = true;
                    break;
                case sf::Keyboard::R:
                    prepareNewGame(GameType::RANDOM);
                    isGameTypeSwitched_ = true;
                    break;
                case sf::Keyboard::Add:
                    if(volume_ < 100) ++volume_;
                    background_.setVolume(volume_);
                    break;
                case sf::Keyboard::Subtract:
                    if(volume_ > 0) --volume_;
                    background_.setVolume(volume_);
                    break;
                case sf::Keyboard::S:
                    isMenuActive_ = true;
                    break;
                default:
                    break;
            }
        }

        //Adjusting impact force by mouse wheel
        else if(events.type == sf::Event::MouseWheelMoved)
        {
            if(events.mouseWheel.delta > 0 && impactForce_ < 100)
            {
                if(impactForce_ < 100) ++impactForce_;
            }
            else if(events.mouseWheel.delta < 0)
            {
                if(impactForce_ > 0) --impactForce_;
            }
        }
        else if(events.type == sf::Event::MouseButtonReleased)
        {
            // Chose cueball by left click
            if(events.mouseButton.button == sf::Mouse::Left)
            {
                if(isBallChoosen_)
                {
                    double dir_angle = atan2(currCursorPos_.y -
                                             balls_[choosenBallIndex_]->y(),
                                             currCursorPos_.x -
                                             balls_[choosenBallIndex_]->x());
                    double vx = impactForce_ * cos(dir_angle);
                    double vy = impactForce_ * sin(dir_angle);
                    balls_[choosenBallIndex_]->push(vx, vy);
                    isLegalHit_ = false;
                    isBallChoosen_ = false;
                    isTurnChecked_ = false;
                    firstPlayerScoreOld_ = firstPlayerScore_;
                    secondPlayerScoreOld_ = secondPlayerScore_;
                    playRandCueHitSound();
                }
                else
                {
                    int x = events.mouseButton.x, y = events.mouseButton.y;
                    for(auto i = 0u; i < balls_.size(); ++i)
                    {
                        if(areAllBallsStopped() && balls_[i]->isInPlay() &&
                                balls_[i]->isCursorInside(x, y))
                        {
                            choosenBallIndex_ = i;
                            isBallChoosen_ = true;
                            if(!balls_[cueBallIndex_]->isInPlay() &&
                               gameType_ != GameType::AMERIKANKA)
                            {
                                cueBallIndex_ = choosenBallIndex_;
                                balls_[choosenBallIndex_]->selectAsCueBall();
                            }
                            break;
                        }
                    }
                }
            }
            else if(events.mouseButton.button == sf::Mouse::Right)
            {
                //Cancel selection by right click
                if(isBallChoosen_)
                    isBallChoosen_ = false;
            }
        }
        else if(events.type == sf::Event::MouseMoved)
        {
            currCursorPos_.x = events.mouseMove.x;
            currCursorPos_.y = events.mouseMove.y;
        }
     }
}

void billards::Game::updatePhase()
{
    if(gameState_ == GameState::PLAYER1_TO_PLAY ||
       gameState_ == GameState::PLAYER2_TO_PLAY)
    {
        for(auto &ball : balls_)
            if(ball->isInPlay())
            {
                //Moving all ball a little bit
                if(ball->move(0.05f))
                   if(sounds_["ball_wall"].getStatus() ==
                           sf::Sound::Status::Stopped)
                        sounds_["ball_wall"].play();
                // Checking for hits
                if(checkPockettHit(*ball))
                {
                    sounds_["ball_pocket"].play();
                    std::cout << "Boom!" << std::endl;
                }
            }
        //Checking for balls collisions
        for(auto i = 0u; i < balls_.size(); ++i)
        {
            for(unsigned int j{i + 1}; j < balls_.size(); ++j)
            {
                if(balls_[i]->isInPlay() && balls_[j]->isInPlay())
                {
                    if(resolveCollision(*balls_[i], *balls_[j]))
                    {
                        sounds_["ball_ball"].play();
                        //Если игрок ударил не по битку то удар не засчитывается в
                        //случае если не играем в американку
                        if(gameType_ != GameType::AMERIKANKA)
                        {
                            isLegalHit_ =
                                    (choosenBallIndex_ == cueBallIndex_);
                        }
                        else
                        {
                           isLegalHit_ = true;
                        }
                    }
                }
            }
        }
        //Если игрок не забил шар то очередь хода переходит партнеру
        if(areAllBallsStopped() && !isTurnChecked_)
        {
            if(gameState_ == GameState::PLAYER1_TO_PLAY)
            {
               int firstPlayerScoreIncrement = firstPlayerScore_ -
                       firstPlayerScoreOld_;
               if(firstPlayerScoreIncrement > 0) {
                   //Если удар был произведен не битком, или
                   //биток не столкнулся с другим шаром до попадания в лузу
                   //то забитые шары переходят на полку сопернику
                   if(!isLegalHit_)
                   {
                       firstPlayerScore_ -= firstPlayerScoreIncrement;
                       secondPlayerScore_ += firstPlayerScoreIncrement;
                       gameState_ = GameState::PLAYER2_TO_PLAY;
                   }
               }
               else if(!isGameTypeSwitched_)
               {
                   gameState_ = GameState::PLAYER2_TO_PLAY;
               }
            }
            else if(gameState_ == GameState::PLAYER2_TO_PLAY)
            {
               int secondPlayerScoreIncrement = secondPlayerScore_ -
                       secondPlayerScoreOld_;
               if(secondPlayerScoreIncrement > 0)
               {
                   if(!isLegalHit_)
                   {
                       secondPlayerScore_ -= secondPlayerScoreIncrement;
                       firstPlayerScore_ += secondPlayerScoreIncrement;
                       gameState_ = GameState::PLAYER1_TO_PLAY;
                   }
               }
               else if(!isGameTypeSwitched_)
               {
                   gameState_ = GameState::PLAYER1_TO_PLAY;
               }
            }
            isTurnChecked_ = true;
        }
        if(!isGameTypeSwitched_)
        {
            if(firstPlayerScore_ > constants::winBallCount)
                gameState_ = GameState::PLAYER1_WON;
            else if(secondPlayerScore_ > constants::winBallCount)
                gameState_ = GameState::PLAYER2_WON;
        }
        else
        {
           isGameTypeSwitched_ = false;
        }
    }
}

void billards::Game::drawPhasePlay()
{
    window_.clear(sf::Color::Black);
    if(isMenuActive_)
    {
        drawMenu();
    }
    else
    {
        drawField();
        for(const auto &ball : balls_)
            ball->draw(window_);
        if(isBallChoosen_ && areAllBallsStopped())
            drawHint();
        drawGameInfo();
        drawInstructions();
        if(gameState_ == GameState::PLAYER1_WON ||
           gameState_ == GameState::PLAYER2_WON)
            drawWinMessage();
    }
}

void billards::Game::drawField() const
{
    window_.draw(tableSprite_);
}

void billards::Game::drawHint() const
{
    sf::Vector2f p1, p2, p3, p4;
    bool isBallsOnTheWay{false};
    calcHintParam(p1, p2, p3, p4, isBallsOnTheWay);
    //Drawing hints
    //Линия направления удара
    sf::VertexArray line12(sf::LinesStrip, 2);
    line12[0].position = p1;
    line12[0].color = sf::Color::Green;
    line12[1].position = p2;
    line12[1].color = sf::Color::Green;
    //Линия отражения
    sf::VertexArray line23(sf::LinesStrip, 2);
    line23[0].position = p2;
    line23[0].color = sf::Color::Red;
    line23[1].position = p3;
    line23[1].color = sf::Color::Red;
    sf::CircleShape circle;
    circle.setOutlineThickness(1);
    circle.setFillColor(sf::Color(0, 0, 0, 0));
    circle.setOutlineColor(sf::Color::Red);
    circle.setRadius(10);
    circle.setOrigin(10, 10);
    circle.setPosition(p2);
    window_.draw(line12);
    window_.draw(line23);
    window_.draw(circle);
    if(isBallsOnTheWay)
    {
        //Cue ball line
        sf::VertexArray line24(sf::LinesStrip, 2);
        line24[0].position = p2;
        line24[0].color = sf::Color::Magenta;
        line24[1].position = p4;
        line24[1].color = sf::Color::Magenta;
        window_.draw(line24);
    }
}

void billards::Game::drawWinMessage() const
{
    std::string sentence = (gameState_ == GameState::PLAYER1_WON) ?
                constants::winMessage1 :
                constants::winMessage2;
    sf::Text text(sf::String(sentence), font_, 60);
    text.setPosition(200, 200);
    text.setColor(sf::Color::Yellow);
    window_.draw(text);
}

void billards::Game::calcHintParam(sf::Vector2f &p1, sf::Vector2f &p2,
                                   sf::Vector2f &p3, sf::Vector2f &p4,
                                   bool &isBallsOnTheWay) const
{
    if(!isBallChoosen_) return;
    constexpr double R {constants::ballRadius};
    p1 = {balls_[choosenBallIndex_]->x(), balls_[choosenBallIndex_]->y()};
    p2 = {constants::fieldLeft + R, p1.y};
    p3 = p2;
    p4 = p2;
    //Определяем коэффициенты прямой характеризующей текущее
    //направление удара
    double k, b, alpha;
    MathFunctions::straightLineCoef(p1, currCursorPos_, k, b, alpha);
    //Определим сразу коэффициенты перпендикулярной направлению удара прямой
    double kn{-1/k}, bn{p1.y - kn * p1.x}; //Проверка деления на ноль
    //Создаем вектор с номерами шаров которые лежат на текущем возможном
    //пути битка
    std::vector<int> ballsOnTheCueBallWay;
    for(unsigned int i{0}; i < balls_.size(); ++i)
    {
        if(i != choosenBallIndex_ && balls_[i]->isInPlay() &&
                MathFunctions::distanceFromPointToLine(k, b, balls_[i]->x(),
                                                       balls_[i]->y()) < 2 * R)
        {
            bool isPointLowerThanNormal{balls_[i]->y() < kn *
                        balls_[i]->x() + bn};
            bool isPointOK{false};
            if(currCursorPos_.y == p1.y)
               isPointOK = (currCursorPos_.x > p1.x) ? balls_[i]->x() > p1.x :
                                                       balls_[i]->x() < p1.x;
            else
               isPointOK = (alpha > -M_PI && alpha < 0) ?
                           isPointLowerThanNormal : !isPointLowerThanNormal;
            if(isPointOK) ballsOnTheCueBallWay.push_back(i);
        }
    }
    isBallsOnTheWay = !ballsOnTheCueBallWay.empty();
    if(isBallsOnTheWay)
    {
        //Сортируем вектор по критерию - кто расположен максимально
        //близко к битку
        std::sort(ballsOnTheCueBallWay.begin(), ballsOnTheCueBallWay.end(),
        [&](auto index1, auto index2)
        {
           double distance1 = MathFunctions::distanceBetweenTwoPoints(
                       p1.x, p1.y, balls_[index1]->x(), balls_[index1]->y());
           double distance2 = MathFunctions::distanceBetweenTwoPoints(
                       p1.x, p1.y, balls_[index2]->x(), balls_[index2]->y());
           return distance1 < distance2;
        });
        sf::Vector2f nearestBallPos {balls_[ballsOnTheCueBallWay[0]]->x(),
                    balls_[ballsOnTheCueBallWay[0]]->y()};
        //Определяем точку пересечния прямой направления удара и окружности
        //двойного радиуса с центром,
        //совпадающим с позицией ближайшего шара
        sf::Vector2f crossPoint1, crossPoint2;
        MathFunctions::crossPointOfCircleAndLine(nearestBallPos, 2 * R,
                                                 k, b, crossPoint1,
                                                 crossPoint2);
        //Определяем какая точка ближе
        double distance1 = MathFunctions::distanceBetweenTwoPoints(
                    p1.x, p1.y, crossPoint1.x, crossPoint1.y);
        double distance2 = MathFunctions::distanceBetweenTwoPoints(
                    p1.x, p1.y, crossPoint2.x, crossPoint2.y);
        p2 = distance1 <= distance2 ? crossPoint1 : crossPoint2;
        //Определяем позицию точки p3 - указывающей направление отлета
        //ударяемого шара
        double k23, b23, alpha23; // Коэффиуиенты прямой 2-3 соединяющей центры
        //битка и ударяемого шара в момент соударения
        //Точки пересечения прямой 2-3 и окружности ценром,
        //совпадающем с позицией
        //ударяемого шара и увеличенным радиусом, который определяет
        //длину линии-подсказки
        //направления ударяемого шара
        sf::Vector2f crossPoint23_1, crossPoint23_2;
        MathFunctions::straightLineCoef(p2, nearestBallPos, k23, b23, alpha23);
        MathFunctions::crossPointOfCircleAndLine(
                    nearestBallPos, 5 * R, k23, b23, crossPoint23_1,
                    crossPoint23_2);
        double distance23_1 = MathFunctions::distanceBetweenTwoPoints(
                    p2.x, p2.y, crossPoint23_1.x, crossPoint23_1.y);
        double distance23_2 = MathFunctions::distanceBetweenTwoPoints(
                    p2.x, p2.y, crossPoint23_2.x, crossPoint23_2.y);
        p3 = distance23_1 >= distance23_2 ? crossPoint23_1 : crossPoint23_2;
        //Определяем положение точки р4
        double k24, b24;
        if(k23 != 0)
        {
            k24 = -1 / k23;
            b24 = p2.y - k24 * p2.x; // Коэффициенты прямой показывающе
            //направление битка после удара
            // Точки пересечения прямой 2-4 и окружности ценром, совпадающем
            //с позицией битка в момент удара и увеличенным радиусом,
            //который определяет длину линии-подсказки
            //направления битка
            sf::Vector2f crossPoint24_1, crossPoint24_2;
            MathFunctions::crossPointOfCircleAndLine(
                        p2, 8 * R, k24, b24, crossPoint24_1, crossPoint24_2);
            double distance14_1 = MathFunctions::distanceBetweenTwoPoints(
                        p1.x, p1.y, crossPoint24_1.x, crossPoint24_1.y);
            double distance14_2 = MathFunctions::distanceBetweenTwoPoints(
                        p1.x, p1.y, crossPoint24_2.x, crossPoint24_2.y);
            if(distance14_1 == distance14_2)
                p4 = p2;
            else
                p4 = (distance14_1 > distance14_2) ? crossPoint24_1 :
                                                     crossPoint24_2;
        }
        else
        {
            p3 = p2;
            p4 = p2;
        }
    }
    else
    {
        //Обрабатываем ситуации когда при текущем направлении удара
        //биток не зацепит ни один из шаров
        //Вычисляем направления отрезков между текущим
        //положением выбранного шара
        // и потенциальными угловыми положениями шаров
        double angle_top_left_lim = atan2(
                    constants::fieldTop + R - p1.y,
                    constants::fieldLeft + R - p1.x);
        double angle_top_right_lim = atan2(
                    constants::fieldTop + R - p1.y,
                    constants::fieldRight - R - p1.x);
        double angle_bottom_right_lim = atan2(
                    constants::fieldBottom - R - p1.y,
                    constants::fieldRight - R - p1.x);
        double angle_bottom_left_lim = atan2(
                    constants::fieldBottom - R - p1.y,
                    constants::fieldLeft + R - p1.x);
        if(alpha == M_PI || alpha == -M_PI)
        {
            p2.y = p1.y;
            p2.x = constants::fieldLeft + R;
            p3 = p2;
        }
        else if(alpha > angle_top_left_lim && alpha < angle_top_right_lim)
        {
            if(fabs(alpha + M_PI_2) < 0.001)
            {
                p2.x = p1.x;
                p2.y = constants::fieldTop + R;
                p3 = p2;
            }
            else
            {
                p2.y = constants::fieldTop + R;
                p2.x = (p2.y - b) / k;
                p3.y = constants::fieldTop + 10 * R;
                p3.x = (-p3.y + p2.y + k * p2.x) / k;
                if(p3.x < constants::fieldLeft) p3.x =
                        constants::fieldLeft;
                if(p3.x > constants::fieldRight) p3.x =
                        constants::fieldRight;
            }
        }else if(alpha >= angle_top_right_lim &&
                 alpha < angle_bottom_right_lim)
        {
            p2.x = constants::fieldRight - R;
            p2.y = k * p2.x + b;
            p3.x = constants::fieldRight - 10 * R;
            p3.y = p2.y - k * (p3.x - p2.x);
            if(p3.y < constants::fieldTop) p3.y =
                    constants::fieldTop;
            if(p3.y > constants::fieldBottom) p3.y =
                    constants::fieldBottom;
        }else if(alpha >= angle_bottom_right_lim &&
                 alpha < angle_bottom_left_lim)
        {
            if(fabs(alpha - M_PI_2) < 0.001)
            {
                p2.x = p1.x;
                p2.y = constants::fieldBottom - R;
                p3 = p2;
            }
            else
            {
                p2.y = constants::fieldBottom - R;
                p2.x = (p2.y - b) / k;
                p3.y = constants::fieldBottom - 10 * R;
                p3.x = (-p3.y + p2.y + k * p2.x) / k;
                if(p3.x < constants::fieldLeft) p3.x =
                        constants::fieldLeft;
                if(p3.x > constants::fieldRight) p3.x =
                        constants::fieldRight;
            }
        }
        else if((alpha > -M_PI && alpha <= angle_top_left_lim) ||
                (alpha >= angle_bottom_left_lim && alpha < M_PI))
        {
            p2.x = constants::fieldLeft + R;
            p2.y = k * p2.x + b;
            p3.x = constants::fieldLeft + 10 * R;
            p3.y = p2.y - k * (p3.x - p2.x);
            if(p3.y < constants::fieldTop) p3.y =
                    constants::fieldTop;
            if(p3.y > constants::fieldBottom) p3.y =
                    constants::fieldBottom;
        }
    }
}

void billards::Game::createBalls(GameType gameType)
{
    balls_.clear();
    switch(gameType)
    {
        case GameType::AMERIKANKA :
            for(int i{0}; i < constants::ballCount; ++i)
            {
                //In this type of the game the cue ball has
                //dark red color and object balls
                //are white
                bool isCueBall{i == 0};
                balls_.emplace_back(std::make_unique<Ball>(
                                        i, 0.f, 0.f,
                                        constants::ballRadius,
                                        constants::ballsArrangement[i].x,
                                        constants::ballsArrangement[i].y,
                                        isCueBall,(isCueBall ?
                                                   constants::darkRedColor :
                                                   sf::Color::White)
                                        )
                                    );
            }
            break;
        case GameType::PIRAMIDA :
            for(int i{0}; i < constants::ballCount; ++i)
            {
                //In this type of the game the cue ball has
                //orange color and object balls
                //are white
                bool isCueBall{i == 0};
                balls_.emplace_back(std::make_unique<Ball>(
                            i, 0.f, 0.f, constants::ballRadius,
                            constants::ballsArrangement[i].x,
                constants::ballsArrangement[i].y, isCueBall,(
                                isCueBall ? constants::orangeColor :
                                            sf::Color::White)));
            }
            break;
        case GameType::POOL :
            for(int i{0}; i < constants::ballCount; ++i)
            {
                bool isCueBall{i == 0};
                sf::Color color;
                if(isCueBall)
                {
                    color = sf::Color::White;
                }
                else if(i >= 1 && i <= 7)
                {
                    color = sf::Color::Red;
                }
                else if(i == 8)
                {
                    color = sf::Color::Black;
                }
                else
                {
                    color = sf::Color::Yellow;
                }
                balls_.emplace_back(std::make_unique<Ball>(
                            i, 0.f, 0.f, constants::ballRadius,
                            constants::ballsArrangement[i].x,
                constants::ballsArrangement[i].y, isCueBall, color));
            }
            break;
        case GameType::RANDOM :
            generateRandomBallSetup();
            break;
    }
}

void billards::Game::generateRandomBallSetup()
{
    balls_.clear();    //Cleare previous verctor of balls
    bool isPosOK = true;
    static const unsigned int maxIterrationCount = 1000;
    unsigned int counter = 0;
    int indexCueBall = 0;
    for(int i{0}; i < constants::ballCount; ++i)
    {
        isPosOK = true;
        counter = 0;
        do
        {
            double randX = (double)(random_.nextInt(36));
            double randY = (double)(random_.nextInt(17));
            bool isCueBall = (i == indexCueBall) ? true : false;
#ifdef DEBUG
            std::cout << "Iterration number " << counter + 1 << std::endl;
#endif
            auto next_ball{std::make_unique<Ball>(
                             i, 0.f, 0.f, constants::ballRadius,
                             constants::fieldLeft +
                             2 * constants::ballRadius + randX * 22,
                             constants::fieldTop +
                             2 * constants::ballRadius + randY * 22,
                             isCueBall, (isCueBall ? constants::orangeColor :
                             sf::Color::White))};
            for(const auto &ball : balls_)
            {
                if(checkCollision(*next_ball, *ball))
                {
                   isPosOK = false;
#ifdef DEBUG
            std::cout << "Collision was detected" << std::endl;
#endif
                   break;
                }
            }
            if(isPosOK)
            {
#ifdef DEBUG
                std::cout << "Choosen position has following coordinates:"
                             " Xrand = " << 2 * constants::ballRadius +
                             randX * 22 << " Yrand = " << 2 *
                             constants::ballRadius + randY * 22 <<
                             std::endl;
#endif
                balls_.emplace_back(std::move(next_ball));
            }
            if(++counter > maxIterrationCount)
            {
                counter = 0;
                balls_.clear();
                i = 0;
                break;
            }
        }
        while(!isPosOK);
#ifdef DEBUG
        std::cout << "The ball number " << i + 1 << " has been installed" <<
                     std::endl;
#endif

    }
    if(balls_.at(indexCueBall)->getColor() != constants::orangeColor)
    {
        balls_.at(indexCueBall)->repaintAsCueBall();
#ifdef DEBUG
        std::cout << "indexCueBall = " << indexCueBall << std::endl;
        std::cout << "Cue ball was repainted" << std::endl;
#endif
    }
#ifdef DEBUG
    std::cout << "Cue ball coordinates : x = " << balls_.at(indexCueBall)->x() <<
                 " y = " << balls_.at(indexCueBall)->y() << std::endl;
#endif
}

void billards::Game::createPockets()
{
    for(auto i = 0u; i < constants::pocketsData1.size(); ++i)
        pockets_.emplace_back(std::make_unique<Pocket>(
                    constants::pocketsData1[i][0],
                    constants::pocketsData1[i][1],
                    constants::pocketsData1[i][2],
                    constants::pocketsData1[i][3]));
}

void billards::Game::loadSprites()
{
    static const std::string pathToBackground{"Resources/Images/mesa.png"};
    tableTexture_.loadFromFile(pathToBackground);
    tableSprite_.setTexture(tableTexture_);
    tableSprite_.setPosition(0, 0);
}

void billards::Game::loadFonts()
{
    static const std::string pathToFont{"Resources/Fonts/DroidSansMono.ttf"};
    font_.loadFromFile(pathToFont);
}

void billards::Game::loadSounds()
{
    static const std::string pathToBGM{"Resources/Sounds/background.ogg"};
    cueHitsSoundBuffers_.clear();
    gameSoundsBuffers_.clear();
    sounds_.clear();
    cueHits_.clear();
    //Загружаем в цикле варианты звуков удара кия
    for(const auto &path : constants::ballSoundsPathes)
    {
        sf::SoundBuffer cueHit;
        cueHit.loadFromFile(path);
        cueHitsSoundBuffers_.push_back(cueHit);
        sf::Sound cueHit_sound;
        cueHit_sound.setVolume(100);
        cueHits_.push_back(cueHit_sound);
    }
    for(unsigned int i{0}; i < cueHits_.size(); ++i)
        cueHits_[i].setBuffer(cueHitsSoundBuffers_[i]);
    //Загружаем остальные звуки
    for(const auto &path : constants::gameSoundsPathes)
    {
        sf::SoundBuffer sound_buffer;
        sound_buffer.loadFromFile(path.second);
        gameSoundsBuffers_.push_back(sound_buffer);
        sf::Sound sound;
        sound.setVolume(100);
        sounds_.insert({path.first, sound});
    }
    for(unsigned int i{0}; i < constants::gameSoundsPathes.size(); ++i)
        sounds_[constants::gameSoundsPathes[i].first].setBuffer(
                    gameSoundsBuffers_[i]);
    //Background music
    background_.openFromFile(pathToBGM);
    background_.setLoop(true);
    background_.setVolume(volume_);
}

void billards::Game::playRandCueHitSound()
{
    int randNum = random_.nextInt(cueHits_.size());
    cueHits_[randNum].play();
}

bool billards::Game::areAllBallsStopped()
{
    for(const auto &ball : balls_)
    {
        if(ball->isInPlay() && ball->v() != 0)
            return false;
    }
    return true;
}

bool billards::Game::checkCollision(double b1_x, double b1_y, double b1_r,
                          double b2_x, double b2_y, double b2_r)
{
    double dx = b2_x- b1_x;
    double dy = b2_y - b1_y;
    double radSum = b1_r + b2_r;
    return sqrt(pow(dx, 2) + pow(dy, 2)) <= radSum;
}

bool billards::Game::checkCollision(Ball &ball1, Ball &ball2)
{
    double dx = ball2.x() - ball1.x();
    double dy = ball2.y() - ball1.y();
    double radSum = ball1.radius() + ball2.radius();
    return sqrt(pow(dx, 2) + pow(dy, 2)) <= radSum;
}

bool billards::Game::checkCollision(Ball &ball1, Ball &ball2, double &overlapX,
                                    double &overlapY, double &angle_rad)
{
    double dx = ball2.x() - ball1.x();
    double dy = ball2.y() - ball1.y();
    angle_rad = atan2(dy , dx);
    double d = sqrt(pow(dx, 2) + pow(dy, 2));
    bool result;
    if(d <= ball1.radius() + ball2.radius())
    {
        double overlap = ball1.radius() + ball2.radius() - d;
        overlapX = -overlap * cos(angle_rad);
        overlapY = -overlap * sin(angle_rad);
        result = true;
    }
    else
    {
        overlapX = 0.f;
        overlapY = 0.f;
        result = false;
    }
    return result;
}

bool billards::Game::resolveCollision(Ball &ball1, Ball &ball2)
{
    if(&ball1 == &ball2) return false;
    double overlapX, overlapY, teta;
    if(!checkCollision(ball1, ball2, overlapX, overlapY, teta)) return false;
    //We calculate velocity components of the first ball
    //reative to the second ball
    double vx_relative = ball1.vx() -  ball2.vx();
    double vy_relative  = ball1.vy() -  ball2.vy();
    double v_relative  = sqrt(pow(vx_relative, 2) + pow(vy_relative, 2));
    //Now we calculate the direction of the first ball
    //relative to the coordinate system connected
    //With second ball as if the second ball were moveless/
    double angle_v_relative {atan2(vy_relative, vx_relative)};
    double ball2_v2_relative {v_relative * cos(angle_v_relative - teta)};
    double ball2_velocity_angle2r {teta};
    double ball2_v2_relative_x {ball2_v2_relative *
                cos(ball2_velocity_angle2r)};
    double ball2_v2_relative_y {ball2_v2_relative *
                sin(ball2_velocity_angle2r)};
    double ball2_v2_absolute_x {ball2_v2_relative_x +  ball2.vx()};
    double ball2_v2_absolute_y {ball2_v2_relative_y +  ball2.vy()};
    //Absolute values and directions of the second ball
    double ball1_v2_relative {v_relative * sin(angle_v_relative - teta)};
    double ball1_velocity_angle2r = ball2_velocity_angle2r + 0.5 * M_PI;
    double ball1_v2_relative_x = ball1_v2_relative *
            cos(ball1_velocity_angle2r);
    double ball1_v2_relative_y = ball1_v2_relative *
            sin(ball1_velocity_angle2r);
    double ball1_v2_absolute_x = ball1_v2_relative_x +  ball2.vx();
    double ball1_v2_absolute_y = ball1_v2_relative_y +  ball2.vy();
    ball1.move(1.01 * overlapX, 1.01 * overlapY);
    // Here we move the first ball to compensate overlap
    //Assignment of the calculated velocity values to colliding balls
    ball1.push(ball1_v2_absolute_x, ball1_v2_absolute_y);
    ball2.push(ball2_v2_absolute_x, ball2_v2_absolute_y);
    return true;
}

bool billards::Game::hasFirstPlayerBalls()
{
#ifdef DEBUG
    std::cout << "Here we check whether the first player has red balls"
                 " on the table" << std::endl;
#endif
    bool result {false};
    for(int i{1}; i < constants::ballCount / 2; ++i)
    {
        if(balls_.at(i)->isInPlay())
        {
            result = true;
            break;
        }
    }
    return result;
}

bool billards::Game::hasSecondPlayerBalls()
{
#ifdef DEBUG
    std::cout << "Here we check whether the second player has yellow "
                 "balls on the table" << std::endl;
#endif
    bool result {false};
    for(int i{9}; i < 16; ++i)
    {
        if(balls_.at(i)->isInPlay())
        {
            result = true;
            break;
        }
    }
    return result;
}

bool billards::Game::checkPockettHit(Ball &ball)
{
    bool result{false};
    static const int poolBlackBallIndex{8};
    int counter = 0;
    for(const auto &pocket : pockets_)
    {
        //std::cout << "Interration number - " << counter + 1 << std::endl;
        if(pocket->isBallInside(ball.x(), ball.y()))
        {
            std::cout << "Ball in the pocket number " << counter << std::endl;
            ball.putBallToThePocket();
            int index = ball.getIndex();
            if(gameState_ == GameState::PLAYER1_TO_PLAY)
            {
                if(gameType_ == GameType::POOL)
                {
                    if((index >= 1 && index < poolBlackBallIndex) ||
                       (index == poolBlackBallIndex && !hasFirstPlayerBalls()))
                    {
                        ++firstPlayerScore_;
                    }
                    else
                    {
                        //If the player pocket the wrong ball we will
                        //set that ball randomly inside the house area
                        setupBallRandomlyInTheHouseArea(index);
                        ball.activate();
                    }
                }
                else
                {
                    ++firstPlayerScore_;
                    std::cout << "First player score was incremented" << std::endl;
                }
            }
            else if(gameState_ == GameState::PLAYER2_TO_PLAY)
            {
                if(gameType_ == GameType::POOL)
                {
                    if((index >= constants::ballCount / 2 + 1 &&
                        index < constants::ballCount) ||
                       (index == poolBlackBallIndex &&
                        !hasSecondPlayerBalls()))
                    {
                        ++secondPlayerScore_;
                    }
                    else
                    {
                        setupBallRandomlyInTheHouseArea(index);
                        ball.activate();
                    }
                }
                else
                {
                    ++secondPlayerScore_;
                }
            }
            result = true;
            break;
        }
        ++counter;
    }

    return result;
}

bool billards::Game::setupBallRandomlyInTheHouseArea(unsigned int index)
{
    bool result = false;
    if(!balls_.empty() && (index < balls_.size()))
    {
        bool isPosOK = true;
        do
        {
            //Magic numbers mean the bounds of the billards table house area
            double randX = 60 + (double)(rand() % 149);
            double randY = 60 + (double)(rand() % 383);
            for(const auto &ball : balls_)
            {
                if(checkCollision(randX, randY, constants::ballRadius,
                                  ball->x(), ball->y(), ball->radius()))
                {
                   isPosOK = false;
                   break;
                }
            }
            if(isPosOK)
            {
                double dx = randX - balls_.at(index)->x();
                double dy = randY - balls_.at(index)->y();
                balls_.at(index)->move(dx, dy);
                balls_.at(index)->stop();
            }
        }
        while(!isPosOK);
        result = true;
    }
    return result;
}

void billards::Game::configureTextLabels()
{
    menuTitle_.setPosition(260, 15);
    menuTitle_.setColor(sf::Color::Magenta);
    menuTitle_.setStyle(sf::Text::Style::Bold | sf::Text::Style::Italic);

    menuWord_.setPosition(350, 85);
    menuWord_.setColor(sf::Color::Red);
    menuWord_.setStyle(sf::Text::Style::Bold);


    menuHint1_.setPosition(170, 420);
    menuHint1_.setColor(sf::Color::Green);

    menuHint2_.setPosition(170, 460);
    menuHint2_.setColor(sf::Color::Green);

    for(auto i = 0u; i < constants::gameMenu.size(); ++i)
    {
        menuItems_.emplace_back(constants::gameMenu.at(i), font_,
                                MENU_ITEMS_FONT_SIZE);
        menuItems_[i].setPosition(200, 200 + i * 40);
        menuItems_[i].setColor(sf::Color(0, 148, 255));
    }

    txtInstructions_.setColor(sf::Color(0, 148, 255));
    txtInstructions_.setPosition(INSTRUCTIONS_X, INSTRUCTIONS_Y);
    txtInstructions_.setStyle(sf::Text::Style::Bold);

    firstPlayerName_.setPosition(10, 510);
    secondPlayerName_.setPosition(10, 550);

    status_.setColor(sf::Color(182, 255, 0));
    status_.setStyle(sf::Text::Style::Bold);
    status_.setPosition(10, 600);

    txtForceAndVolume_.setColor(sf::Color::Green);
    txtForceAndVolume_.setStyle(sf::Text::Style::Bold);
    txtForceAndVolume_.setPosition(425, 510);
}

void billards::Game::menuItemActivate(int item)
{
    switch (item) {
        case 0:
            prepareNewGame(GameType::AMERIKANKA);
            break;
        case 1:
            prepareNewGame(GameType::PIRAMIDA);
            break;
        case 2:
            prepareNewGame(GameType::POOL);
            break;
        case 3:
            prepareNewGame(GameType::RANDOM);
            break;
        case 4:
            window_.close();
            break;
        default:
            break;
    }
    isGameTypeSwitched_ = true;
    isMenuActive_ = false;
}

void billards::Game::drawMenu() const
{
    window_.draw(menuTitle_);
    window_.draw(menuWord_);
    for(auto i = 0u; i < menuItems_.size(); ++i)
    {
        if(selectedMenuItem_ == i)
        {
            menuItems_.at(i).setColor(sf::Color::Yellow);
        }
        else
        {
            menuItems_.at(i).setColor(sf::Color(0, 148, 255));
        }
        window_.draw(menuItems_[i]);
    }
    window_.draw(menuHint1_);
    window_.draw(menuHint2_);
}

void billards::Game::prepareNewGame(GameType gameType)
{
    gameType_ = gameType;
    balls_.clear();
    firstPlayerScore_ = 0;
    secondPlayerScore_ = 0;
    cueBallIndex_ = 0;
    choosenBallIndex_ = 0;
    impactForce_ = DEFAUT_IMPACT_FORCE;
    isLegalHit_ = false;
    isBallChoosen_ = false;
    createBalls(gameType);
    gameState_ = GameState::PLAYER1_TO_PLAY;
}

void billards::Game::drawGameInfo() const
{
    drawPlayerNames();
    drawPlayersScore();
    drawGameType();
    drawForceAndVolume();
    updateWindowTitle();
}

void billards::Game::drawPlayerNames() const
{
    if(gameState_ == GameState::PLAYER1_TO_PLAY)
    {
        firstPlayerName_.setColor(sf::Color::Green);
        secondPlayerName_.setColor(sf::Color::Red);
    }
    else
    {
        firstPlayerName_.setColor(sf::Color::Red);
        secondPlayerName_.setColor(sf::Color::Green);
    }
    window_.draw(firstPlayerName_);
    window_.draw(secondPlayerName_);
}

void billards::Game::drawPlayersScore() const
{
    static const int scoreBallRadius{10}, gapBetweenPoints{15},
    firstPlayerPointRowTop{530}, secondPlayerPointRowTop{570};
    for(int i{0}; i < firstPlayerScore_; ++i)
    {
        sf::CircleShape point;
        point.setFillColor(sf::Color::Yellow);
        point.setRadius(scoreBallRadius);
        point.setOrigin(scoreBallRadius, scoreBallRadius);
        point.setPosition(firstPlayerName_.getGlobalBounds().left +
                          firstPlayerName_.getGlobalBounds().width +
                          scoreBallRadius + i * (point.getRadius() +
                                                 gapBetweenPoints),
                          firstPlayerPointRowTop);
        window_.draw(point);
    }

    for(int i{0}; i < secondPlayerScore_; ++i)
    {
        sf::CircleShape point;
        point.setFillColor(sf::Color::Yellow);
        point.setRadius(scoreBallRadius);
        point.setOrigin(scoreBallRadius, scoreBallRadius);
        point.setPosition(firstPlayerName_.getGlobalBounds().left +
                          firstPlayerName_.getGlobalBounds().width +
                          scoreBallRadius + i * (point.getRadius() +
                                                 gapBetweenPoints),
                          secondPlayerPointRowTop);
        window_.draw(point);
    }
}

void billards::Game::drawGameType() const
{
    std::string type{""};
    switch (gameType_) {
    case GameType::AMERIKANKA :
        type = "AMERIKANKA";
        break;
    case GameType::PIRAMIDA :
        type = "RUSSIAN PYRAMID";
        break;
    case GameType::POOL :
        type = "POOL";
        break;
    case GameType::RANDOM :
        type = "RANDOM SETUP";
        break;
    default:
        break;
    }
    status_.setString(type);
    window_.draw(status_);
}

void billards::Game::drawForceAndVolume() const
{
    std::stringstream ss;
    ss << "Impact force: " << impactForce_ << "% " << " Volume: " <<
          volume_ << "%" << std::endl;
    txtForceAndVolume_.setString(ss.str());
    window_.draw(txtForceAndVolume_);
}

void billards::Game::updateWindowTitle() const
{
    std::stringstream score_ss;
    score_ss << firstPlayerScore_ << "/" << secondPlayerScore_;
    window_.setTitle(constants::titleOfProgram +
                     " SCORE: "+ score_ss.str());
}

void billards::Game::drawInstructions() const
{
    window_.draw(txtInstructions_);
}
