#include "game.h"
#include <memory>

int main()
{
    std::unique_ptr<billards::Game> upGame{std::make_unique<billards::Game>()};
    upGame->run();
    return 0;
}
