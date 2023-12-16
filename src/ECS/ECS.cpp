#include "ECS.h"

int IComponent::nextId = 0;

///////////////////////////////////
// Entity methods implementation //
///////////////////////////////////

int Entity::GetId() const
{
  return id;
}

///////////////////////////////////
// System methods implementation //
///////////////////////////////////

void System::AddEntityToSytem(Entity entity)
{
  entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
  for (int i = 0; i < entities.size(); i++)
  {
    if (entities.at(i) == entity)
    {
      entities.pop_back();
      return;
    }
  }
}

std::vector<Entity> System::GetSystemEntities() const
{
  return entities;
}

const Signature &System::GetComponentSignature() const
{
  return componentSignature;
}

/////////////////////////////////////
// Registry methods implementation //
/////////////////////////////////////

Entity Registry::CreateEntity()
{
  int entityId;
  entityId = numEntities++;

  Entity entity(entityId);
  entity.registry = this;
  entitiesToBeAdded.insert(entity);

  if (entityId >= entityComponentSignatures.size())
  {
    entityComponentSignatures.resize(entityId + 1);
  }

  Logger::Log("Entity created with id: " + std::to_string(entityId));

  return entity;
}

void Registry::AddEntityToSystems(Entity entity)
{
  const auto entityId = entity.GetId();
  const auto &entityComponentSignature = entityComponentSignatures[entityId];

  for (auto &system : systems)
  {
    const auto &systemComponentSignature = system.second->GetComponentSignature();
    bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

    if (isInterested)
    {
      system.second->AddEntityToSytem(entity);
    }
  }
}

void Registry::Update()
{
  // Add the entities that are waiting to be created to the active systems
  for (auto entity : entitiesToBeAdded)
  {
    AddEntityToSystems(entity);
  }
  entitiesToBeAdded.clear();

  // Remove the entities that are waiting to be killed from the active systems
}