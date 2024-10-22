#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ECS/ECS.h"
#include "Components.h"
#include "Logger/Logger.h"

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

        void Update(SDL_Renderer *renderer) {
            for (auto entity : GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();

                SDL_Rect objRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    sprite.width,
                    sprite.height
                };

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &objRect);
            }
        }
};

#endif
