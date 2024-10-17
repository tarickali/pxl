#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;


class Game {
    private:
        bool isRunning;
        int millisecsPreviousFrame;
        SDL_Window *window;
        SDL_Renderer *renderer;

    public:
        Game();
        ~Game();

        void Initialize();
        void Setup();
        void Run();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();

        // TODO: Optimize to use a different datatype
        int windowWidth;
        int windowHeight;
};

#endif