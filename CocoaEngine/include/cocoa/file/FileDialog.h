#ifndef COCOA_ENGINE_FILE_DIALOG_H
#define COCOA_ENGINE_FILE_DIALOG_H
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
		COCOA bool getOpenFileName(
			const std::string& initialPath,
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} });

		COCOA bool getOpenFolderName(
			const std::string& initialPath,
			FileDialogResult& result);

		COCOA bool getSaveFileName(
			const std::string& initialPath,
			FileDialogResult& result,
			std::vector<std::pair<std::string, std::string>> extensionFilters = { {"All Files", "*.*"} },
			std::string extToAppend = "");

		COCOA void displayMessageBox(const std::string& title, const std::string& message);
	};
}

#endif
