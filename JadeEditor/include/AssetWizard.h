#pragma once
#include "externalLibs.h"
#include "jade/core/AssetManager.h"
#include "jade/file/IFileDialog.h"

namespace Jade
{
	class AssetWizard
	{
	public:
		static void ImGui();
		static void BeginImport() { s_IsImporting = true; }
		static void NewProject() { s_CreatingProject = true; }

	private:
		AssetWizard();

		static void ChooseFile();
		static void ImGuiImportSpritesheet();
		static void ImGuiImportSprite();

	private:
		static bool s_CreatingProject;
		static bool s_IsImporting;
		static bool s_FileChosen;
		static FileDialogResult s_FileImporting;
		static char s_TmpFilename[256];
		static AssetType s_TypeOfImport;
	};
}