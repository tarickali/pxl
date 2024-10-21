#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

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
// The system processes entities that contain a specific signature.
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
// Pool
////////////////////////////////////////////////////////////////////////////////
// A pool is a vector of objects of type T
////////////////////////////////////////////////////////////////////////////////
class IPool {
    public:
        virtual ~IPool() {} ;
};

template <typename T>
class Pool : public IPool {
    private:
        std::vector<T> data;

    public:
        Pool(int size = 100) { data.resize(size); }
        virtual ~Pool() = default;

        bool IsEmpty() const { return data.empty(); }
        int GetSize() const { return data.size(); }
        void Resize(int n) { data.reserve(n); }
        void Clear() { data.clear(); }
        void Add(T object) { data.push_back(object); }
        void Set(int index, T object) { data[index] = object; }
        T &Get(int index) { return static_cast<T &>(data[index]); }
        T &operator [](unsigned int index) { return data[index]; }
};

////////////////////////////////////////////////////////////////////////////////
// World
////////////////////////////////////////////////////////////////////////////////
// The world manages the creation and destruction of entities, systems, and
// components.
////////////////////////////////////////////////////////////////////////////////
class World {
    private:
        unsigned int numEntities = 0;

        std::set<Entity> entitiesToBeCreated;
        std::set<Entity> entitiesToBeDestroyed;

        // Vector of component pools, each pool contains all the data for a
        // certain component type.
        // [Vector index = component type id]
        // [Pool index = entity id]
        std::vector<IPool *> componentPools;

        // Vector of component signatures per entity, saying which component
        // is turned "on" for each entity.
        // [Vector index = entity id]
        std::vector<Signature> entityComponentSignatures;

        std::unordered_map<std::type_index, System *> systems;

    public:
        World() = default;
        ~World() = default;

        // Entity management
        Entity CreateEntity();
        // void DestroyEntity(Entity entity);

        // Component management
        template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs &&...args);
        template <typename TComponent> void RemoveComponent(Entity entity);
        template <typename TComponent> bool HasComponent(Entity entity) const;

        // System management
        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs &&...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem() const;
        template <typename TSystem> TSystem &GetSystem() const;

        // Checks the component signature of an entity and add the entity to the
        // systems that are interested in it 
        void AddEntityToSystems(Entity entity);

        void Update();

};

////////////////////////////////////////////////////////////////////////////////
// Template Implementations
////////////////////////////////////////////////////////////////////////////////

// System
template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

// World
template <typename TComponent, typename ...TArgs>
void World::AddComponent(Entity entity, TArgs &&...args) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId()

    // Resize componentPools if necessary to accomadate new component
    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    // Add new component pool if necessary
    if (!componentPools[componentId]) {
        Pool<TComponent> *newComponentPool = new Pool<TComponent>();
        componentPools[componentId] = newComponentPool;
    }
    
    // Get the component pool
    Pool<TComponent> *componentPool = componentPools[componentId];

    // Resize componentPool if necessary to accomadate new entity
    if (entityId >= componentPool->GetSize()) {
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void World::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();    
    entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool World::HasComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TSystem, typename ...TArgs>
void World::AddSystem(TArgs &&...args) {
    TSystem *newSystem(new TSystem(std::forward<TArgs>(args)...));
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void World::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool World::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> 
TSystem &World::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

#endif