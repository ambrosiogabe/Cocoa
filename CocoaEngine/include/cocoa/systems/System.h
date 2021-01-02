#pragma once
#include "externalLibs.h"

#include "cocoa/events/Event.h"

namespace Cocoa
{

	class Scene;

	class COCOA System
	{
	public:
		System(const char* name, Scene* scene)
			: m_Name(name), m_Scene(scene)
		{
		}


		virtual void Start()
		{

		}

		virtual void Update(float dt)
		{

		}

		virtual void EditorUpdate(float dt)
		{

		}

		virtual void Render()
		{

		}

		virtual void OnEvent(Event& e)
		{

		}

		const char* GetName()
		{
			return m_Name;
		}

	private:
		const char* m_Name = "";

	protected:
		Scene* m_Scene = nullptr;
	};
}