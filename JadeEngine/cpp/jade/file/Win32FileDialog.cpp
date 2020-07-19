#include "jade/file/FileDialog.h"
#include "jade/core/Application.h"

#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Jade
{
	bool Win32FileDialog::IGetOpenFileName(
		const std::string& initialPath,
		std::string& outFile,
		std::vector<std::pair<std::string, std::string>> extensionFilters,
		std::string extToAppend)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetNativeWindow());
		HWND wnd = glfwGetWin32Window(window);

		std::string filters = "";
		for (auto pair : extensionFilters)
		{
			filters += pair.first + '\0';
			filters += pair.second + '\0';
		}
		filters += '\0';

		OPENFILENAMEA dialogStruct;
		dialogStruct.lStructSize = sizeof(OPENFILENAMEA);
		dialogStruct.hwndOwner = wnd;
		dialogStruct.hInstance = NULL;
		dialogStruct.lpstrFilter = filters.c_str();
		dialogStruct.lpstrCustomFilter = NULL;
		dialogStruct.nMaxCustFilter = 0;
		dialogStruct.nFilterIndex = 0;
		// Initially.e only place in two bytes so that we get the size required, then we will rerun the command 
		// and get t. appropriate files selected by the user See: https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea
		dialogStruct.lpstrFile = new char[256]; // Output filename
		dialogStruct.lpstrFile[0] = NULL;
		dialogStruct.nMaxFile = 256;
		dialogStruct.lpstrFileTitle = NULL;// (LPSTR)initialPath.c_str();
		dialogStruct.nMaxFileTitle = 0; // initialPath.length();
		dialogStruct.lpstrInitialDir = NULL; // initialPath.c_str();
		dialogStruct.lpstrTitle = "My Custom Title";
		dialogStruct.Flags = 0; // TODO: Consider adding flags to this function
		dialogStruct.nFileOffset = 0;
		dialogStruct.nFileExtension = 0;
		dialogStruct.lpstrDefExt = NULL;

		bool result = GetOpenFileNameA(&dialogStruct);
		if (result)
		{
			outFile = std::string(dialogStruct.lpstrFile);
			if (dialogStruct.nFileExtension == 0)
			{
				outFile += extToAppend;
			}
		}
		delete dialogStruct.lpstrFile;

		return result;
	}

	bool Win32FileDialog::IGetSaveFileName(
		const std::string& initialPath,
		std::string& outFile,
		std::vector<std::pair<std::string, std::string>> extensionFilters,
		std::string extToAppend)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetNativeWindow());
		HWND wnd = glfwGetWin32Window(window);

		std::string filters = "";
		for (auto pair : extensionFilters)
		{
			filters += pair.first + '\0';
			filters += pair.second + '\0';
		}
		filters += '\0';

		OPENFILENAMEA dialogStruct;
		dialogStruct.lStructSize = sizeof(OPENFILENAMEA);
		dialogStruct.hwndOwner = wnd;
		dialogStruct.hInstance = NULL;
		dialogStruct.lpstrFilter = filters.c_str();
		dialogStruct.lpstrCustomFilter = NULL;
		dialogStruct.nMaxCustFilter = 0;
		dialogStruct.nFilterIndex = 0;
		// Initially.e only place in two bytes so that we get the size required, then we will rerun the command 
		// and get t. appropriate files selected by the user See: https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea
		dialogStruct.lpstrFile = new char[256]; // Output filename
		dialogStruct.lpstrFile[0] = NULL;
		dialogStruct.nMaxFile = 256;
		dialogStruct.lpstrFileTitle = NULL;// (LPSTR)initialPath.c_str();
		dialogStruct.nMaxFileTitle = 0; // initialPath.length();
		dialogStruct.lpstrInitialDir = NULL; // initialPath.c_str();
		dialogStruct.lpstrTitle = NULL;
		dialogStruct.Flags = 0; // TODO: Consider adding flags to this function
		dialogStruct.nFileOffset = 0;
		dialogStruct.nFileExtension = 0;
		dialogStruct.lpstrDefExt = NULL;

		bool result = GetSaveFileNameA(&dialogStruct);
		if (result)
		{
			outFile = std::string(dialogStruct.lpstrFile);
			if (dialogStruct.nFileExtension == 0)
			{
				outFile += extToAppend;
			}
		}
		delete dialogStruct.lpstrFile;

		return result;
	}
}