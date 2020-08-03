#include "jade/file/IFileDialog.h"
#include "jade/core/Application.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <commdlg.h>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <knownfolders.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Jade
{
	bool Win32FileDialog::IGetOpenFileName(
		const std::string& initialPath,
		FileDialogResult& fileResult,
		std::vector<std::pair<std::string, std::string>> extensionFilters)
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
			fileResult.filepath = std::string(dialogStruct.lpstrFile);
			fileResult.filename = fileResult.filepath.c_str() + dialogStruct.nFileOffset;
			fileResult.extension = fileResult.filepath.c_str() + dialogStruct.nFileExtension;
		}
		delete dialogStruct.lpstrFile;

		return result;
	}

	bool Win32FileDialog::IGetSaveFileName(
		const std::string& initialPath,
		FileDialogResult& fileResult,
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
			fileResult.filepath = std::string(dialogStruct.lpstrFile);
			if (dialogStruct.nFileExtension == 0)
			{
				dialogStruct.nFileExtension = (WORD)fileResult.filepath.size();
				fileResult.filepath += extToAppend;
			}
			fileResult.filename = fileResult.filepath.c_str() + dialogStruct.nFileOffset;
			fileResult.extension = fileResult.filepath.c_str() + dialogStruct.nFileExtension;
		}
		delete dialogStruct.lpstrFile;

		return result;
	}

	bool Win32FileDialog::IGetOpenFolderName(
		const std::string& initialPath,
		FileDialogResult& fileResult)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetNativeWindow());
		HWND wnd = glfwGetWin32Window(window);

		LPWSTR g_path{};
		::IFileDialog* pfd;
		
		bool result = true;
		if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
		{	
			DWORD dwOptions;
			if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
			{
				pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
			}

			IShellItem* psi;
			SHGetKnownFolderItem(FOLDERID_RecycleBinFolder, KF_FLAG_DEFAULT, nullptr, IID_PPV_ARGS(&psi));
			pfd->SetDefaultFolder(psi);
			psi->Release();

			if (SUCCEEDED(pfd->Show(NULL)))
			{
				IShellItem* psi;
				if (SUCCEEDED(pfd->GetResult(&psi)))
				{
					if (!SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &g_path)))
					{
						MessageBoxA(NULL, "GetIDListName() failed", NULL, NULL);
						result = false;
					}
					else
					{
						char* tmp = new char[256];
						wcstombs(tmp, g_path, 256);
						fileResult.filepath = std::string(tmp);
						Log::Info("Selected folder: %s", fileResult.filepath.c_str());
						delete[] tmp;
					}
					psi->Release();
				}
			}
			pfd->Release();
		}

		return result;
	}
}