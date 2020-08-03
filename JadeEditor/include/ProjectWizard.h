#pragma once
#include "externalLibs.h"
#include "jade/renderer/Texture.h"

namespace Jade
{
	class ProjectWizard
	{
	public:
		ProjectWizard();
		~ProjectWizard();

		void Update(float dt);
		void ImGui();

	private:
		Texture* m_JadeLogo = nullptr;
		glm::vec2 m_IdealSize;
		glm::vec2 m_TexturePos;
		glm::vec2 m_VersionPos;
		glm::vec2 m_CreateProjButtonPos;
		glm::vec2 m_OpenProjectButtonPos;
		glm::vec2 m_ButtonSize;
		glm::vec2 m_Padding{ 10.0f, 20.0f };
	};
}