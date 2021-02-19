#pragma once

#include "externalLibs.h"

namespace Cocoa
{
	struct FileDialogResult
	{
		std::string filepath;
		const char* filename;
		const char* extension;
	};

	namespace FileDialog
	{
		COCOA bool GetOpenFileName(
			const std::string& initialPath,
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} });

		COCOA bool GetOpenFolderName(
			const std::string initialPath,
			FileDialogResult& result);

		COCOA bool GetSaveFileName(
			const std::string& initialPath,
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} },
			std::string extToAppend = "");

		COCOA void DisplayMessageBox(const std::string& title, const std::string& message);
	};
}