#pragma once

//#include "Jade.h"

#include "jade/systems/System.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/events/KeyEvent.h"
#include "jade/events/MouseEvent.h"

namespace Jade
{
	class LevelEditorSystem : public System
	{
	public:
		LevelEditorSystem(const char* name)
			: System(name)
		{
		}

		virtual void Update(entt::registry& registry, float dt) override;
		virtual void Start(entt::registry& registry) override;
		virtual void ImGui(entt::registry& registry) override;
		virtual void OnEvent(Event& e) override;

	private:
		bool HandleKeyPress(KeyPressedEvent& e);
		bool HandleKeyRelease(KeyReleasedEvent& e);
		bool HandleMouseButtonPressed(MouseButtonPressedEvent& e);
		bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool HandleMouseScroll(MouseScrolledEvent& e);

	private:
		float m_KeyDebounceTime = 0.1f;
		float m_KeyDebounceLeft = 0.0f;

		bool m_IsDragging = false;
		bool m_ControlModifierPressed = false;

		glm::vec3 m_DragOffset = glm::vec3();
		glm::vec3 m_CameraSpeed = glm::vec3(0, 0, 0);

		//GizmoController m_GizmoController;
	};
}