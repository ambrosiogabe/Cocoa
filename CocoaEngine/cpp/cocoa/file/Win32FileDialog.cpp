// This file is only built on windows systems
#ifdef _WIN32
#include "cocoa/file/FileDialog.h"
#include "cocoa/core/Application.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <commdlg.h>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <knownfolders.h>


#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#undef GetOpenFileName
#undef GetSaveFileName
#undef ERROR

namespace Cocoa
{
	namespace FileDialog
	{
		static char ResultBuffer[MAX_PATH];

		bool getOpenFileName(
			const std::string& initialPath,
			FileDialogResult& fileResult,
			std::vector<std::pair<std::string, std::string>> extensionFilters)
		{
			GLFWwindow* window = static_cast<GLFWwindow*>(Application::get()->getWindow()->getNativeWindow());
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
			dialogStruct.lpstrFile = ResultBuffer; // Output filename
			dialogStruct.lpstrFile[0] = NULL;
			dialogStruct.nMaxFile = MAX_PATH;
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

			return result;
		}

		bool getSaveFileName(
			const std::string& initialPath,
			FileDialogResult& fileResult,
			std::vector<std::pair<std::string, std::string>> extensionFilters,
			std::string extToAppend)
		{
			GLFWwindow* window = static_cast<GLFWwindow*>(Application::get()->getWindow()->getNativeWindow());
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
			dialogStruct.lpstrFile = ResultBuffer; // Output filename
			dialogStruct.lpstrFile[0] = NULL;
			dialogStruct.nMaxFile = 256;
			dialogStruct.lpstrFileTitle = NULL;// (LPSTR)initialPath.c_str();
			dialogStruct.nMaxFileTitle = 0; // initialPath.length();
			dialogStruct.lpstrInitialDir = initialPath.c_str(); 
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

			return result;
		}

		bool getOpenFolderName(
			const std::string& initialPath,
			FileDialogResult& fileResult)
		{
			GLFWwindow* window = static_cast<GLFWwindow*>(Application::get()->getWindow()->getNativeWindow());
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
				SHGetKnownFolderItem(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, IID_PPV_ARGS(&psi));
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
							wcstombs(ResultBuffer, g_path, 256);
							fileResult.filepath = std::string(ResultBuffer);
							Logger::Info("Selected folder: %s", fileResult.filepath.c_str());
						}
						psi->Release();
					}
					else
					{
						result = false;
					}
				}
				else
				{
					result = false;
				}
				pfd->Release();
			}
			else
			{
				result = false;
			}

			return result;
		}

		void displayMessageBox(const std::string& title, const std::string& message)
		{
			GLFWwindow* window = static_cast<GLFWwindow*>(Application::get()->getWindow()->getNativeWindow());
			HWND wnd = glfwGetWin32Window(window);
			int messageBoxId = MessageBoxA(wnd, message.c_str(), title.c_str(), MB_ICONEXCLAMATION | MB_OK);
		}
	}
}
#endif