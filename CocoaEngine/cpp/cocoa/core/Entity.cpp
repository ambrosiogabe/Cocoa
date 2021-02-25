#include "cocoa/core/Entity.h"
#include "cocoa/core/Application.h"

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

		Entity CreateEntity(entt::entity raw)
		{
			return Entity{ raw };
		}

		void SetScene(SceneData* scene)
		{
			m_Scene = scene;
		}

		SceneData* GetScene()
		{
			return m_Scene;
		}

		Entity CreateNull()
		{
			return Null;
		}
	}

	bool operator==(const Entity& a, const Entity& b)
	{
		return a.Handle == b.Handle;
	}

	bool operator==(Entity& a, Entity& b)
	{
		return a.Handle == b.Handle;
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