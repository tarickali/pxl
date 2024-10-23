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

class CollisionSystem : public System {
    public:
        CollisionSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update() {
            struct Box {
                int left, right, top, bottom;
            };

            const auto entities = GetSystemEntities();

            for (auto i=entities.begin(); i!=entities.end(); i++) {
                auto a = *i;

                const auto aTransform = a.GetComponent<TransformComponent>();
                const auto aCollider = a.GetComponent<BoxColliderComponent>();

                for (auto j=std::next(i); j!=entities.end(); j++) {
                    auto b = *j;

                    const auto bTransform = b.GetComponent<TransformComponent>();
                    const auto bCollider = b.GetComponent<BoxColliderComponent>();

                    bool collisionHappened = checkkAABBCollision(
                        aTransform.position.x + aCollider.offset.x * aTransform.scale.x,
                        aTransform.position.y + aCollider.offset.y * aTransform.scale.y,
                        aCollider.width * aTransform.scale.x,
                        aCollider.height * aTransform.scale.y,
                        bTransform.position.x + bCollider.offset.x * bTransform.scale.x,
                        bTransform.position.y + bCollider.offset.y * bTransform.scale.y,
                        bCollider.width * bTransform.scale.x,
                        bCollider.height * bTransform.scale.y
                    );

                    if (collisionHappened) {
                        // TODO: Emit an event

                        Logger::Log("Entity " + std::to_string(a.GetId()) + " is collidiing with " + std::to_string(b.GetId()));
                    }
                }
            } 
        }

        bool checkkAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
            return aX < bX + bW && aX + aW > bX && aY < bY + bH && aY + aH > bY;
        }
};

class RenderCollisionSystem : public System {
    public:
        RenderCollisionSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(SDL_Renderer *renderer) {
            for (auto entity : GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto collider = entity.GetComponent<BoxColliderComponent>();

                SDL_Rect rect = {
                    static_cast<int>(transform.position.x + collider.offset.x * transform.scale.x),
                    static_cast<int>(transform.position.y + collider.offset.y * transform.scale.y),
                    static_cast<int>(collider.width * transform.scale.x),
                    static_cast<int>(collider.height * transform.scale.y)
                };

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }

};

#endif
