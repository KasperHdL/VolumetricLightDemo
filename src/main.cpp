#include "Game.hpp"
#include "Engine.hpp"

Game game = Game();

int main() {
    Engine engine(1920, 1080);

    return engine.initialize(&game);
}

