#include "jade/core/Entity.h"
#include "jade/core/Application.h"

namespace Jade
{
	Entity Entity::Null = Entity();
	Scene* Entity::s_Scene = nullptr;

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle)
	{
		Log::Assert((s_Scene != nullptr), "Scene cannot be null to construct an entity. Did you forget to initialize entity at scene change?");
	}

	Entity::Entity(const entt::entity& other)
	{
		Log::Assert((s_Scene != nullptr), "Scene cannot be null to construct an entity. Did you forget to initialize entity at scene change?");
		m_EntityHandle = other;
	}

	Entity::Entity(entt::entity& other)
	{
		Log::Assert((s_Scene != nullptr), "Scene cannot be null to construct an entity. Did you forget to initialize entity at scene change?");
		m_EntityHandle = other;
	}

	Entity::Entity()
	{
		m_EntityHandle = entt::null;
	}

	void Entity::SetScene(Scene* scene)
	{
		s_Scene = scene;
	}
}