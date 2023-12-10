#pragma once
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

const unsigned int MAX_COMPONENTS = 32;

// Signature:
// We use a bitset (1s and 0s) to keep track of which components an entity has,
// and also helps keep track of which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
protected:
  static int nextId;
};

// Used to assign a unique id to a component type
template <typename TComponent>
class Component : public IComponent
{
  // returns the unique id of Component<T>
  static int GetId()
  {
    static auto id = nextId++;
    return id;
  }
};

class Entity
{
private:
  int id;

public:
  Entity(int id) : id(id){}; // id(id) to initialize id automatically
  Entity(const Entity &entity) = default;
  int GetId() const;

  // operator overloading to compare 2 entities easily
  Entity &operator=(const Entity &other) = default;
  bool operator==(const Entity &other) const { return id == other.id; }
  bool operator!=(const Entity &other) const { return id != other.id; }
  bool operator>(const Entity &other) const { return id > other.id; }
  bool operator<(const Entity &other) const { return id < other.id; }
};

// The system processes entities that contain a specific signature
class System
{
private:
  Signature componentSignature;
  std::vector<Entity> entities;

public:
  System() = default;
  ~System() = default;

  void AddEntityToSytem(Entity entity);
  void RemoveEntityFromSystem(Entity entity);
  std::vector<Entity> GetSystemEntities() const;
  const Signature &GetComponentSignature() const;

  // Defines the component type that entities must have to be considered by the system
  template <typename TComponent>
  void RequireComponent();
};

// A pool is a vector of objects of type T
class IPool
{
public:
  virtual ~IPool() {}
};

template <typename T>
class Pool : public IPool
{
private:
  std::vector<T> data;

public:
  Pool(int size = 100)
  {
    data.resize(size);
  }
  virtual ~Pool() = default;

  bool isEmpty() const { return data.empty(); }
  int GetSize() const { return data.size(); }
  void Resize(int newSize) { data.resize(newSize); }
  void Clear() { data.clear(); }
  void Add(T object) { data.push_back(object); }
  void Set(int index, T object) { data[index] = object; }
  T &Get(int index) const { return static_cast<T &>(data[index]); }
  T &operator[](unsigned int index) const { return data[index]; };
};

// The registry manages the creation and destruction of entities, add systems, and components
class Registry
{
private:
  // Keep track of how many entities were added to the scene
  int numEntities = 0;

  // Each pool contains all the data for a certain component type
  // [Vector index = component type id]
  // [Pool index = entity id]
  std::vector<IPool *> componentPools;

  // The signature lets us know which components are turned "on" for an entity
  // [index = entity id]
  std::vector<Signature> entityComponentSignatures;

  // Map of active systems
  // [index = system type id]
  std::unordered_map<std::type_index, System *> systems;

  // Entities awaiting creation in the next frame (registry::update)
  std::set<Entity> entitiesToBeAdded;

  // Entities awaiting destruction in the next frame (registry::update)
  std::set<Entity> entitiesToBeKilled;

public:
  Registry() = default;

  void Update();

  Entity CreateEntity();
  void AddEntityToSystem(Entity entity);

  // Function template to add a component of type T to a given entity
  template <typename TComponent, typename... TArgs>
  void AddComponent(Entity entity, TArgs &&...args);

  template <typename TComponent>
  void RemoveComponent(Entity entity);

  template <typename TComponent>
  bool HasComponent(Entity entity);
};

template <typename TComponent>
void System::RequireComponent()
{
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs &&...args)
{
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  if (componentId >= componentPools.size())
  {
    componentPools.resize(componentId + 1, nullptr);
  }

  // If the pool for this component type doesn't exist, create it
  if (!componentPools[componentId])
  {
    Pool<TComponent> *newPool = new Pool<TComponent>();
    componentPools[componentId] = newPool;
  }

  // Get the pool of component values for that component type
  Pool<TComponent> *componentPool = Pool<TComponent>(componentPools[componentId]);

  if (entityId >= componentPool->GetSize())
  {
    componentPool->Resize(numEntities);
  }

  // Create the component and forward the various parameters to the constructor
  TComponent newComponent(std::forward<TArgs>(args)...);

  // Add the component to the pool
  componentPool->Set(entityId, newComponent);

  // Update the signature of the entity to show that it has the component
  entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  // Get the pool of component values for that component type
  Pool<TComponent> *componentPool = Pool<TComponent>(componentPools[componentId]);

  // Remove the component from the pool
  componentPool->Set(entityId, TComponent());

  // Update the signature of the entity to show that it no longer has the component
  entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity)
{
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  return entityComponentSignatures[entityId].test(componentId);
}