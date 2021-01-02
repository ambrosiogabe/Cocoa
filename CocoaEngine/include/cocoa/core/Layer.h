#pragma once
#include "externalLibs.h"

#include "cocoa/events/Event.h"

namespace Cocoa
{

	class Scene;

	class COCOA Layer
	{
	public:
		Layer(Scene* scene, const std::string& name = "Layer")
			: m_Scene(scene), m_DebugName(name)
		{
		}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnRender() {}

		virtual void OnEvent(Event& e) {}

	protected:
		std::string m_DebugName;
		Scene* m_Scene = nullptr;
	};
}