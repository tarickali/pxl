#include <iostream>
#include <SDL2/SDL.h>

#include "Game.h"

Game::Game() {
    // TODO:...
    std::cout << "Game constructor called." << std::endl;
    isRunning = false;
}

Game::~Game() {
    std::cout << "Game deconstructor called." << std::endl;

}

void Game::Initialize() {
    int err = SDL_Init(SDL_INIT_EVERYTHING);
    if (err != 0) {
        std::cerr << "Error: Could not initialize SDL." << std::endl;
        return;
    }

    // NOTE: this only sets window width and height to fullscreen dimensions,
    // but does not change video mode to real fullscreen
    // SDL_DisplayMode displayMode;
    // SDL_GetCurrentDisplayMode(0, &displayMode);
    // windowWidth = displayMode.w;
    // windowHeight = displayMode.h;

    // This sets the game resolution to be 800x600 regardless of screen size.
    windowWidth = 800;
    windowHeight = 600;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        std::cerr << "Error: Could not create SDL window." << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        std::cerr << "Error: Could not create SDL renderer." << std::endl;
        return;
    }

    // NOTE: This changes the video mode to real full screen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

void Game::Run() {
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                break;
        }

    }


}

void Game::Update() {
    // TODO: Update game objects...
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    // TODO: Render all game objects...

    SDL_RenderPresent(renderer);
}


void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}