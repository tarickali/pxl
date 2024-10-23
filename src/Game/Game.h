#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>

const int FPS = 120;
const int MILLISECS_PER_FRAME = 1000 / FPS;


class Game {
    private:
        bool isRunning;
        bool isDebug;

        int millisecsPreviousFrame;
        SDL_Window *window;
        SDL_Renderer *renderer;

        std::unique_ptr<World> world;
        std::unique_ptr<AssetStore> assetStore;

    public:
        Game();
        ~Game();

        void Initialize();
        void LoadLevel(int level);
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