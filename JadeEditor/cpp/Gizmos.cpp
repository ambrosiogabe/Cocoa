#include "Gizmos.h"

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/renderer/Camera.h"
#include "jade/core/Application.h"
#include "jade/util/JMath.h"

namespace Jade
{
	// =================================================================================================
	// Gizmo
	// =================================================================================================
	Gizmo::Gizmo(Sprite* sprite, glm::vec3 startPosition, glm::vec2 halfSize, float spriteRotation)
	{
		m_Active = false;
		m_HalfSize = halfSize;
		m_Position = startPosition;
		m_SpriteRotation = spriteRotation;
		m_TexCoordMin = sprite->m_TexCoords[0];
		m_TexCoordMax = sprite->m_TexCoords[2];
		m_TexId = sprite->m_Texture->GetId();
	}

	void Gizmo::Render()
	{
		if (m_Visible)
		{
			if (m_Active)
			{
				DebugDraw::AddSprite(m_TexId, m_HalfSize * 2.0f, JMath::Vector2From3(m_Position), { 1.0f, 1.0f, 1.0f }, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
			}
			else
			{
				DebugDraw::AddSprite(m_TexId, m_HalfSize * 2.0f, JMath::Vector2From3(m_Position), { 0.6f, 0.6f, 0.6f }, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
			}
		}
	}

	void Gizmo::GizmoManipulateTranslate(Transform& transform)
	{

	}

	// =================================================================================================
	// Gizmo Controller
	// =================================================================================================
	GizmoSystem::GizmoSystem(const char* name)
		: System(name)
	{
		m_Texture = std::unique_ptr<Texture>(new Texture("assets/images/gizmos.png", false));
		m_Spritesheet = std::unique_ptr<Spritesheet>(new Spritesheet(m_Texture.get(), 16, 40, 9, 0));

		float nLength = 16.0f;
		float lLength = 40.0f;
		float sLength = 20.0f;
		m_HzMove = Gizmo(&m_Spritesheet->GetSprite(1), glm::vec3(), { lLength, nLength }, 90.0f);
		m_VtMove = Gizmo(&m_Spritesheet->GetSprite(4), glm::vec3(), { nLength, lLength }, 180.0f);
		m_FreeMove = Gizmo(&m_Spritesheet->GetSprite(0), glm::vec3(), { sLength, sLength }, 0.0f);

		m_HzScale = Gizmo(&m_Spritesheet->GetSprite(2), glm::vec3(), { lLength, nLength }, 90.0f);
		m_VtScale = Gizmo(&m_Spritesheet->GetSprite(5), glm::vec3(), { nLength, lLength }, 180.0f);
		m_FreeScale = Gizmo(&m_Spritesheet->GetSprite(2), glm::vec3(), { sLength, sLength }, 0.0f);
	}

	void GizmoSystem::Update(const entt::registry& registry, float dt)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();
		if (entt::to_integral(activeEntity) != entt::to_integral(entt::null))
		{
			int start = 0;
			int end = 3;
			if (m_Mode == GizmoMode::Translate)
			{
				start = 4;
				end = 6;
			}

			const Transform& entityTransform = registry.get<Transform>(activeEntity);
			for (int i = start; i < end; i++)
			{
				m_Gizmos[i].m_Position = entityTransform.m_Position;
				m_Gizmos[i].Render();
			}
		}
	}

	void GizmoSystem::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(std::bind(&GizmoSystem::HandleKeyPress, this, std::placeholders::_1));
		dispatcher.Dispatch<KeyReleasedEvent>(std::bind(&GizmoSystem::HandleKeyRelease, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&GizmoSystem::HandleMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&GizmoSystem::HandleMouseButtonReleased, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&GizmoSystem::HandleMouseScroll, this, std::placeholders::_1));
	}

	bool GizmoSystem::HandleKeyPress(KeyPressedEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleKeyRelease(KeyReleasedEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleMouseScroll(MouseScrolledEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		Camera* camera = Application::Get()->GetScene()->GetCamera();
		glm::vec2 mousePosWorld = camera->ScreenToOrtho();

		for (int i = 0; i < 6; i++)
		{
			Gizmo gizmo = m_Gizmos[i];
			if (Physics2D::PointInBox(mousePosWorld, gizmo.m_HalfSize, gizmo.m_Position, 0.0f))
			{
				gizmo.m_Active = true;
				m_ActiveGizmo = i;
				break;
			}
		}

		return false;
	}

	bool GizmoSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e)
	{

		return false;
	}
}