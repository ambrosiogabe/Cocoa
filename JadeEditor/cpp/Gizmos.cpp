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
	Gizmo::Gizmo(Sprite* sprite, glm::vec3 startPosition, glm::vec3 offset, glm::vec2 halfSize, float spriteRotation)
	{
		m_Active = false;
		m_HalfSize = halfSize;
		m_Position = startPosition;
		m_Offset = offset;
		m_SpriteRotation = spriteRotation;
		m_TexCoordMin = sprite->m_TexCoords[0];
		m_TexCoordMax = sprite->m_TexCoords[2];
		m_Texture = sprite->m_Texture;
	}

	void Gizmo::Render()
	{
		if (m_Active)
		{
			DebugDraw::AddSprite(m_Texture, m_HalfSize * 2.0f, JMath::Vector2From3(m_Position), { 0.0f, 0.0f, 1.0f }, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
		}
		else
		{
			DebugDraw::AddSprite(m_Texture, m_HalfSize * 2.0f, JMath::Vector2From3(m_Position), { 1.0f, 1.0f, 1.0f }, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
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

		float width = 8.0f;
		float height = 20.0f;

		float squareWidth = 4;
		float squareHeight = 10;

		float hzOffsetX = 15;
		float hzOffsetY = -10;
		float squareOffsetX = 0;
		float squareOffsetY = 2;
		float vtOffsetX = -8;
		float vtOffsetY = 12;
		m_HzMove = Gizmo(&m_Spritesheet->GetSprite(1), glm::vec3(), { hzOffsetX, hzOffsetY, 0.0f}, { width, height }, -90.0f);
		m_VtMove = Gizmo(&m_Spritesheet->GetSprite(4), glm::vec3(), { vtOffsetX, vtOffsetY, 0.0f }, { width, height }, 0.0f);
		m_FreeMove = Gizmo(&m_Spritesheet->GetSprite(0), glm::vec3(), { squareOffsetX, squareOffsetY, 0.0f }, { squareWidth, squareHeight }, 0.0f);

		m_HzScale = Gizmo(&m_Spritesheet->GetSprite(2), glm::vec3(), { hzOffsetX, hzOffsetY, 0.0f }, { width, height }, 90.0f);
		m_VtScale = Gizmo(&m_Spritesheet->GetSprite(5), glm::vec3(), { vtOffsetX, vtOffsetY, 0.0f }, { width, height }, 180.0f);
		m_FreeScale = Gizmo(&m_Spritesheet->GetSprite(2), glm::vec3(), { squareOffsetX, squareOffsetY, 0.0f }, { squareWidth, squareHeight }, 0.0f);
	}

	void GizmoSystem::ImGui(entt::registry& registry)
	{
		float nLength = 8.0f;
		float lLength = 20.0f;
		float sLength = 10.0f;

		float negOffset = -10.0f;
		float posOffsetX = 5.0f;
		float posOffsetY = 15.0f;

		ImGui::Begin("Gizmo debug");
		ImGui::DragFloat2("Free Move Offset:", &m_FreeMove.m_Offset[0]);
		ImGui::DragFloat2("Free Move HalfSize: ", &m_FreeMove.m_HalfSize[0]);
		static int fId = 4;
		if (ImGui::InputInt("Free Move Tex Id", &fId))
		{
			m_FreeMove = Gizmo(&m_Spritesheet->GetSprite(fId), glm::vec3(), { negOffset, 0.0f, 0.0f }, { nLength, lLength }, 0.0f);
		}

		ImGui::DragFloat2("HzMove Offset:", &m_HzMove.m_Offset[0]);
		ImGui::DragFloat2("HzMove HalfSize: ", &m_HzMove.m_HalfSize[0]);
		static int hId = 4;
		if (ImGui::InputInt("HzMove Tex Id", &hId))
		{
			m_HzMove = Gizmo(&m_Spritesheet->GetSprite(hId), glm::vec3(), { negOffset, 0.0f, 0.0f }, { nLength, lLength }, 0.0f);
		}

		ImGui::DragFloat2("VtMove Offset:", &m_VtMove.m_Offset[0]);
		ImGui::DragFloat2("VtMove HalfSize: ", &m_VtMove.m_HalfSize[0]);
		static int vId = 4;
		if (ImGui::InputInt("VtMove Tex Id", &vId))
		{
			m_VtMove = Gizmo(&m_Spritesheet->GetSprite(vId), glm::vec3(), { negOffset, 0.0f, 0.0f }, { nLength, lLength }, 0.0f);
		}
		ImGui::End();
	}

	void GizmoSystem::Update(entt::registry& registry, float dt)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();
		if (entt::to_integral(activeEntity) != entt::to_integral(entt::null))
		{
			int start = 0;
			int end = 3;
			if (m_Mode == GizmoMode::Translate)
			{
				start = 3;
				end = 6;
			}

			Camera* camera = Application::Get()->GetScene()->GetCamera();
			glm::vec2 mousePosWorld = camera->ScreenToOrtho();
			const Transform& entityTransform = registry.get<Transform>(activeEntity);
			for (int i = start; i < end; i++)
			{
				Gizmo gizmo = m_Gizmos[i];
				if (Physics2D::PointInBox(mousePosWorld, gizmo.m_HalfSize, gizmo.m_Position, 0.0f))
				{
					gizmo.m_Active = true;
					m_ActiveGizmo = i;
					break;
				}
				gizmo.m_Position = entityTransform.m_Position + gizmo.m_Offset;
				gizmo.Render();
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
		dispatcher.Dispatch<MouseMovedEvent>(std::bind(&GizmoSystem::HandleMouseMoved, this, std::placeholders::_1));
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

	bool GizmoSystem::HandleMouseMoved(MouseMovedEvent& e)
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
}