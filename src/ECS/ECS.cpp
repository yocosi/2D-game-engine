#include "ECS.h"

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