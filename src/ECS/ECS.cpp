#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const
{
  return id;
}

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

Entity Registry::CreateEntity()
{
  int entityId;
  entityId = numEntities++;

  Entity entity(entityId);
  entitiesToBeAdded.insert(entity);

  Logger::Log("Entity created with id: " + std::to_string(entityId));

  return entity;
}

void Registry::Update()
{
}