#pragma once
#include <bitset>
#include <vector>

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

class Registry
{
  // TODO:...
};

template <typename TComponent>
void System::RequireComponent()
{
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}