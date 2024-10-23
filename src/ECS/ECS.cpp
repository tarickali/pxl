#include "ECS.h"
#include "../Logger/Logger.h"

#include <algorithm>

unsigned int IComponent::nextId = 0;

////////////////////////////////////////////////////////////////////////////////
// Entity
////////////////////////////////////////////////////////////////////////////////
unsigned int Entity::GetId() const {
    return id;
}

void Entity::Destroy() {
    world->DestroyEntity(*this);
}

////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////
void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [&entity](Entity other) {
                return entity == other;
            }
        ),
        entities.end()
    );
}

std::vector<Entity> System::GetSystemEntities() const {
    return entities;
}

const Signature &System::GetComponentSignature() const {
    return componentSignature;
}

////////////////////////////////////////////////////////////////////////////////
// World
////////////////////////////////////////////////////////////////////////////////
Entity World::CreateEntity() {
    unsigned int entityId;

    if (freeIds.empty()) {
        entityId = numEntities++;
        if (entityId >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.world = this;
    entitiesToBeCreated.insert(entity);

    Logger::Log("Entity created with id = " + std::to_string(entityId));

    return entity;
}

void World::DestroyEntity(Entity entity) {
    entitiesToBeDestroyed.insert(entity);
    Logger::Log("Entity destroyed with id = " + std::to_string(entity.GetId()));
}

void World::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();

    // Match entityComponentSignature <---> systemComponentSignature
    const auto &entityComponentSignature = entityComponentSignatures[entityId];

    for (auto &system : systems) {
        const auto &systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void World::RemoveEntityFromSystems(Entity entity) {
    for (auto &system : systems) {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void World::Update() {
    // Add the entities that are waiting to be created to the active Systems
    // Remove the entities that are waiting to be created to the active Systems

    for (auto entity : entitiesToBeCreated) {
        AddEntityToSystems(entity);
    }
    entitiesToBeCreated.clear();

    for (auto entity : entitiesToBeDestroyed) {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();

        // Make the entity id available to be reused
        freeIds.push_back(entity.GetId());
    }
    entitiesToBeDestroyed.clear();
}