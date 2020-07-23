#include "AssetWizard.h"
#include "jade/file/IFileDialog.h"

namespace Jade
{
	void AssetWizard::ImGui()
	{
		if (m_IsImporting)
		{
			if (!m_FileChosen)
			{
				ChooseFile();
			}
			else
			{
				switch (m_TypeOfImport)
				{
				case AssetType::Spritesheet:
					ImGuiImportSpritesheet();
					break;
				}
			}
		}
	}

	void AssetWizard::ChooseFile()
	{
		if (IFileDialog::GetOpenFileName(".", m_FileImporting))
		{
			m_FileChosen = true;
		}
		else
		{
			m_IsImporting = false;
		}
	}

	void AssetWizard::ImGuiImportSpritesheet()
	{
		Log::Info("Importing spritesheet here...");
		m_IsImporting = false;
	}
}