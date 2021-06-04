#include "cocoa/core/Entity.h"
#include "cocoa/core/Application.h"
#include "cocoa/scenes/Scene.h"

namespace Cocoa
{
	namespace NEntity
	{
		// Internal Variables
		static Entity Null = { entt::null };
		static SceneData* m_Scene = nullptr;

		Entity CreateEntity(SceneData* scene)
		{
			return Scene::CreateEntity(*scene);
		}

		Entity createEntity(entt::entity raw)
		{
			return Entity{ raw };
		}

		void setScene(SceneData* scene)
		{
			m_Scene = scene;
		}

		SceneData* getScene()
		{
			return m_Scene;
		}

		Entity createNull()
		{
			return Null;
		}
	}

	bool operator==(const Entity& a, const Entity& b)
	{
		return a.handle == b.handle;
	}

	bool operator==(Entity& a, Entity& b)
	{
		return a.handle == b.handle;
	}

	bool operator!=(const Entity& a, const Entity& b)
	{
		return !(a == b);
	}

	bool operator!=(Entity& a, Entity& b)
	{
		return !(a == b);
	}
}
