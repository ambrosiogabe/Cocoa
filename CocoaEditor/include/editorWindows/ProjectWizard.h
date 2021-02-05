#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/renderer/Texture.h"

namespace Cocoa
{
	class ProjectWizard
	{
	public:
		ProjectWizard() = delete;
		~ProjectWizard() = delete;

		static void Init();
		static void ImGui();

		static void CreateProjectImGui(bool& windowOpen);

	private:
		static Texture s_JadeLogo;
		static glm::vec2 s_IdealSize;
		static glm::vec2 s_TexturePos;
		static glm::vec2 s_VersionPos;
		static glm::vec2 s_CreateProjButtonPos;
		static glm::vec2 s_OpenProjectButtonPos;
		static glm::vec2 s_ButtonSize;
		static glm::vec2 s_Padding;

		static bool s_CreatingProject;
		static CPath s_NewProjectPath;
		static char s_TmpFilename[256];
	};
}