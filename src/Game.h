#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>


class Game {
    private:
        bool isRunning;
        SDL_Window *window;
        SDL_Renderer *renderer;

    public:
        Game();
        ~Game();

        void Initialize();
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