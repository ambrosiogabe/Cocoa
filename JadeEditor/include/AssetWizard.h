#pragma once
#include "externalLibs.h"
#include "jade/core/AssetManager.h"

namespace Jade
{
	class AssetWizard
	{
	public:
		void ImGui();

	private:
		void ChooseFile();
		void ImGuiImportSpritesheet();
		void ImGuiPromptEditExisting(const char* filename, AssetType type);

	private:
		bool m_IsImporting;
		bool m_FileChosen;
		std::string m_FileImporting;
		AssetType m_TypeOfImport;
	};
}