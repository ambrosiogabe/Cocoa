#pragma once

#include "externalLibs.h"

namespace Jade
{
	class IFileDialog
	{
	public:
		static void Init();
		static void Destroy();

		static bool GetOpenFileName(
			const std::string& initialPath, 
			std::string& outFile,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} },
			std::string extToAppend = "")
		{
			return Get()->IGetOpenFileName(initialPath, outFile, extensionFilters, extToAppend);
		}

		static bool GetSaveFileName(
			const std::string& initialPath,
			std::string& outPath,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} },
			std::string extToAppend = "")
		{
			return Get()->IGetSaveFileName(initialPath, outPath, extensionFilters, extToAppend);
		}

	protected:
		// ---------------------------------------------------------------------------
		// Interface definitions
		// ---------------------------------------------------------------------------
		virtual bool IGetOpenFileName(
			const std::string& initialPath, 
			std::string& outPath, 
			std::vector<std::pair<std::string, std::string>> extensionFilters,
			std::string extToAppend) = 0;

		virtual bool IGetSaveFileName(
			const std::string& initialPath,
			std::string& outPath,
			std::vector<std::pair<std::string, std::string>> extensionFilters,
			std::string extToAppend) = 0;

	protected:
		static IFileDialog* Get();

	private:
		static IFileDialog* s_Instance;
	};


	// ---------------------------------------------------------------------------
	// Windows Platform class
	// --------------------------------------------------------------------------- 
#ifdef _WIN32
	class Win32FileDialog : public IFileDialog
	{
	public:
		static Win32FileDialog* Create()
		{
			return new Win32FileDialog();
		}

	protected:
		virtual bool IGetOpenFileName(
			const std::string& initialPath, 
			std::string& outFile,
			std::vector<std::pair<std::string, std::string>> extensionFilters,
			std::string extToAppend) override;

		virtual bool IGetSaveFileName(
			const std::string& initialPath,
			std::string& outPath,
			std::vector<std::pair<std::string, std::string>> extensionFilters,
			std::string extToAppend) override;
	};
#endif
}