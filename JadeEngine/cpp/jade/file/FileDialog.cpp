#include "jade/file/FileDialog.h"

namespace Jade
{
	IFileDialog* IFileDialog::s_Instance = nullptr;
	IFileDialog* IFileDialog::Get()
	{
		if (s_Instance == nullptr)
		{
#ifdef _WIN32
			s_Instance = Win32FileDialog::Create();
#endif
		}
		return s_Instance;
	}
}