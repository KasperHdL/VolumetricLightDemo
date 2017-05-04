#include "Game.hpp"
#include "Engine.hpp"

Game game = Game();

int main() {
    Engine engine(1440, 900);

    return engine.initialize(&game);
}

