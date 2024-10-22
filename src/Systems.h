#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ECS/ECS.h"
#include "Components.h"
#include "Logger/Logger.h"
#include "AssetStore/AssetStore.h"

#include <algorithm>
#include <SDL2/SDL.h>

class MovementSystem : public System {
    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        void Update(double deltaTime) {
            // TODO:
            // Loop all entities that the system is interested in 
            for (auto entity : GetSystemEntities()) {
                // TODO:
                // Update entity position based on its velocity every frame of the game loop.
                auto &transform = entity.GetComponent<TransformComponent>();
                const auto &rigidbody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;
                Logger::Log(
                    "Entity id = " +
                    std::to_string(entity.GetId()) +
                    " position is now (" +
                    std::to_string(transform.position.x) +
                    ", " +
                    std::to_string(transform.position.y) + ")"
                );
            }    
        }
};

class RenderSystem : public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &assetStore) {
            // Sort all the entities of our system by z-index
            auto entities = GetSystemEntities();
            std::sort(entities.begin(), entities.end(), [](const Entity &a, const Entity &b) {
                return a.GetComponent<SpriteComponent>().zIndex < b.GetComponent<SpriteComponent>().zIndex;
            });

            for (auto entity : entities) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();

                auto texture = assetStore->GetTexture(sprite.assetId);

                SDL_Rect srcRect = sprite.srcRect;

                SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
                };

                SDL_RenderCopyEx(
                    renderer,
                    texture,
                    &srcRect,
                    &dstRect,
                    transform.rotation,
                    NULL,
                    SDL_FLIP_NONE
                );
            }
        }
};

class AnimationSystem : public System {
    public:
        AnimationSystem() {
            RequireComponent<SpriteComponent>();
            RequireComponent<AnimationComponent>();
        }

        void Update() {
            for (auto entity : GetSystemEntities()) {
                auto &animation = entity.GetComponent<AnimationComponent>();
                auto &sprite = entity.GetComponent<SpriteComponent>();

                animation.currentFrame = (
                    (SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000
                ) % animation.numFrames;

                sprite.srcRect.x = animation.currentFrame * sprite.width;
            }
        }
};

#endif
