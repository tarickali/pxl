#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "ECS/ECS.h"

class MovementSystem : public System {
    public:
        MovementSystem() {
            // TODO:
            // RequireComponent<TransformComponent>();
            // RequireComponent<VelocityComponent>();
        }

        void Update() {
            // TODO:
            // Loop all entities that the system is interested in 
            // for (auto entity: GetEntities()) {
                // TODO:
                // Update entity position based on its velocity every frame of the game loop.
            // }
        }
};

#endif