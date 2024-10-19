#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Signature
////////////////////////////////////////////////////////////////////////////////
// We use a bitset to keep track of which components an entity has. It also
// helps keep track of which entities a system is intereseted in.
////////////////////////////////////////////////////////////////////////////////
const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

////////////////////////////////////////////////////////////////////////////////
// Entity
////////////////////////////////////////////////////////////////////////////////
// An Entity is essential an ID that represents a game object.
////////////////////////////////////////////////////////////////////////////////
class Entity {
    private:
        unsigned int id;

    public:
        Entity(int id) : id(id) {};
        Entity(const Entity &entity) = default;
        unsigned int GetId() const;

        Entity &operator =(const Entity &other) = default;
        bool operator ==(const Entity &other) const { return id == other.id; }
        bool operator !=(const Entity &other) const { return id != other.id; }
};

////////////////////////////////////////////////////////////////////////////////
// Component
////////////////////////////////////////////////////////////////////////////////
// A Component is pure data
////////////////////////////////////////////////////////////////////////////////
struct IComponent {
    protected:
        static int nextId;
};

// Used to assign a unique id to a component type
template <typename T>
class Component : public IComponent {
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};

////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////
class System {
    private:
        Signature componentSignature;
        std::vector<Entity> entities;

    public:
        System() = default;
        ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        const Signature &GetComponentSignature() const;

        // Defines the component type that entities must have to be considered by the system
        template <typename TComponent> void RequireComponent();
};

////////////////////////////////////////////////////////////////////////////////
// World
////////////////////////////////////////////////////////////////////////////////
class World {
    private:
        int numEntities = 0;

    public:
        World() = default;

        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        void AddSystem();
        void AddComponent();
};

////////////////////////////////////////////////////////////////////////////////
// Template Implementations
////////////////////////////////////////////////////////////////////////////////
template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

#endif