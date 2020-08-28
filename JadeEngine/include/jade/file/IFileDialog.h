#pragma once

#include "externalLibs.h"

namespace Jade
{
	struct FileDialogResult
	{
		std::string filepath;
		const char* filename;
		const char* extension;
	};

	class JADE IFileDialog
	{
	public:
		static void Init();
		static void Destroy();

		static bool GetOpenFileName(
			const std::string& initialPath,
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} })
		{
			return Get()->IGetOpenFileName(initialPath, result, extensionFilters);
		}

		static bool GetOpenFolderName(
			const std::string initialPath,
			FileDialogResult& result)
		{
			return Get()->IGetOpenFolderName(initialPath, result);
		}

		static bool GetSaveFileName(
			const std::string& initialPath,
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} },
			std::string extToAppend = "")
		{
			return Get()->IGetSaveFileName(initialPath, result, extensionFilters, extToAppend);
		}

	protected:
		// ---------------------------------------------------------------------------
		// Interface definitions
		// ---------------------------------------------------------------------------
		virtual bool IGetOpenFileName(
			const std::string& initialPath, 
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters) = 0;

		virtual bool IGetSaveFileName(
			const std::string& initialPath,
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters,
			std::string extToAppend) = 0;

		virtual bool IGetOpenFolderName(
			const std::string& initialPath,
			FileDialogResult& result) = 0;

	protected:
		static IFileDialog* Get();

	private:
		static IFileDialog* s_Instance;
	};


	// ---------------------------------------------------------------------------
	// Windows Platform class
	// --------------------------------------------------------------------------- 
#ifdef _WIN32
	class JADE Win32FileDialog : public IFileDialog
	{
	public:
		static Win32FileDialog* Create()
		{
			return new Win32FileDialog();
		}

	protected:
		virtual bool IGetOpenFileName(
			const std::string& initialPath, 
			FileDialogResult& outFile,
			std::vector<std::pair<std::string, std::string>> extensionFilters) override;

		virtual bool IGetSaveFileName(
			const std::string& initialPath,
			FileDialogResult& outPath,
			std::vector<std::pair<std::string, std::string>> extensionFilters,
			std::string extToAppend) override;

		virtual bool IGetOpenFolderName(
			const std::string& initialPath,
			FileDialogResult& result) override;
	};
#endif
}