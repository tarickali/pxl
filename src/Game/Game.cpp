#include "Game.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Components.h"
#include "../Systems.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>


Game::Game() {
    isRunning = false;
    isDebug = false;

    world = std::make_unique<World>();
    assetStore = std::make_unique<AssetStore>();

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
                if (event.key.keysym.sym == SDLK_d) {
                    isDebug = !isDebug;
                }
                break; 
        }
    }
}

void Game::LoadLevel(int level) {
    // Add the systems need to be by our game
    world->AddSystem<MovementSystem>();
    world->AddSystem<RenderSystem>();
    world->AddSystem<AnimationSystem>();
    world->AddSystem<CollisionSystem>();
    world->AddSystem<RenderCollisionSystem>();

    // Adding asets to the asset store
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

    ///////////////////////////////////////////////////////////////////////////
    // Load the tilemap
    ///////////////////////////////////////////////////////////////////////////
    // Add the tilemap texture to the asset store

    // Set tile parameters
    int tileSize = 32;
    double tileScale = 2.0;

    // Open the map file for the level layout
    std::fstream mapFile("./assets/tilemaps/jungle.map", std::ios::in);
    if (!mapFile) {
        std::cerr << "Error opening file!" << std::endl;
        throw;
    }

    // Read the level layout
    std::string line;
    int y = 0;
    while (std::getline(mapFile, line)) {
        std::stringstream ss(line);
        std::string value;

        int x = 0;
        while (std::getline(ss, value, ',')) {
            int srcRectY = (value[0] - '0') * tileSize;
            int srcRectX = (value[1] - '0') * tileSize;

            Entity tile  = world->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileSize * tileScale), y * (tileSize * tileScale)), glm::vec2(tileScale, tileScale));
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);

            x += 1;
        }
        y += 1;
    }
    mapFile.close();

    Entity chopper = world->CreateEntity();
    chopper.AddComponent<TransformComponent>(
        glm::vec2(10.0, 10.0),
        glm::vec2(1.0, 1.0),
        0.0
    );
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 10, true);

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    Entity radar = world->CreateEntity();
    radar.AddComponent<TransformComponent>(
        glm::vec2(displayMode.w - 74.0, 10.0),
        glm::vec2(1.0, 1.0),
        0.0
    );
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = world->CreateEntity();
    tank.AddComponent<TransformComponent>(
        glm::vec2(10.0, 10.0),
        glm::vec2(1.0, 1.0),
        0.0
    );
    tank.AddComponent<RigidBodyComponent>(
        glm::vec2(20.0, 0.0)
    );
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));

    Entity truck = world->CreateEntity();
    truck.AddComponent<TransformComponent>(
        glm::vec2(500.0, 10.0),
        glm::vec2(1.0, 1.0),
        0.0
    );
    truck.AddComponent<RigidBodyComponent>(
        glm::vec2(-30.0, 0.0)
    );
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
}

void Game::Setup() {
    LoadLevel(1);
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
    world->GetSystem<AnimationSystem>().Update();
    world->GetSystem<CollisionSystem>().Update();

    world->Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to be updated
    world->GetSystem<RenderSystem>().Update(renderer, assetStore);

    if (isDebug) {
        world->GetSystem<RenderCollisionSystem>().Update(renderer);
    }

    SDL_RenderPresent(renderer);
}


void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}