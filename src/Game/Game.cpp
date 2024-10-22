#include "Game.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Components.h"
#include "../Systems.h"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>


Game::Game() {
    isRunning = false;
    world = std::make_unique<World>();

    Logger::Log("Game constructor called.");
}

Game::~Game() {
    Logger::Log("Game deconstructor called.");

}

void Game::Initialize() {
    int err = SDL_Init(SDL_INIT_EVERYTHING);
    if (err != 0) {
        Logger::Error("Error: Could not initialize SDL.");
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
        Logger::Error("Error: Could not create SDL window.");
        return;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        Logger::Error("Error: Could not create SDL renderer.");
        return;
    }

    // NOTE: This changes the video mode to real full screen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

void Game::Run() {
    Setup();

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

void Game::Setup() {
    world->AddSystem<MovementSystem>();
    world->AddSystem<RenderSystem>();

    Entity tank = world->CreateEntity();
    Entity truck = world->CreateEntity();

    tank.AddComponent<TransformComponent>(
        glm::vec2(10.0, 30.0),
        glm::vec2(1.0, 1.0),
        0.0
    );
    tank.AddComponent<RigidBodyComponent>(
        glm::vec2(40.0, 0.0)
    );
    tank.AddComponent<SpriteComponent>(10, 10);

    truck.AddComponent<TransformComponent>(
        glm::vec2(100.0, 100.0),
        glm::vec2(1.0, 1.0),
        0.0
    );
    truck.AddComponent<RigidBodyComponent>(
        glm::vec2(0.0, 50.0)
    );
    truck.AddComponent<SpriteComponent>(10, 50);
}

void Game::Update() {
    // If we are too faste, waste some time until we reach time MILLISECS_PER_FRAME
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    // NOTE: using a while-loop uses too much processor time
    // while (!SDL_TICKS_PASSED(SDL_GetTicks(), millisecsPreviousFrame + MILLISECS_PER_FRAME));

    // The difference in ticks since the last frame in seconds
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    // Store the current frame time
    millisecsPreviousFrame = SDL_GetTicks();

    // Update game objects
    world->GetSystem<MovementSystem>().Update(deltaTime);
    world->Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to be updated
    world->GetSystem<RenderSystem>().Update(renderer);

    SDL_RenderPresent(renderer);
}


void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}