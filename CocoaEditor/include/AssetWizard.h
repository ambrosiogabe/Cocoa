#pragma once
#include "externalLibs.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/file/IFileDialog.h"

namespace Cocoa
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
		static uint32 s_TypeOfImport;
	};
}