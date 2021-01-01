#pragma once

#include "cocoa/systems/System.h"
#include "cocoa/events/KeyEvent.h"
#include "cocoa/events/MouseEvent.h"

namespace Cocoa
{
	class ScriptSystem;
	class LevelEditorSystem : public System
	{
	public:
		LevelEditorSystem(const char* name, Scene* scene, ScriptSystem* scriptSystem)
			: System(name, scene)
		{
			m_ScriptSystem = scriptSystem;
		}

		virtual void EditorUpdate(float dt) override;
		virtual void Start() override;
		virtual void OnEvent(Event& e) override;

	private:
		bool HandleKeyPress(KeyPressedEvent& e);
		bool HandleKeyRelease(KeyReleasedEvent& e);
		bool HandleMouseButtonPressed(MouseButtonPressedEvent& e);
		bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool HandleMouseScroll(MouseScrolledEvent& e);

	private:
		ScriptSystem* m_ScriptSystem;

		float m_KeyDebounceTime = 0.1f;
		float m_KeyDebounceLeft = 0.0f;

		bool m_IsDragging = false;
		bool m_ControlModifierPressed = false;

		glm::vec3 m_OriginalDragClickPos = glm::vec3();
		glm::vec3 m_OriginalCameraPos = glm::vec3();
	};
}