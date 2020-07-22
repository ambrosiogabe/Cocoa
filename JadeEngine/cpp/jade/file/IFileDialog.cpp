#include "jade/file/IFileDialog.h"
#include "jade/util/Log.h"

namespace Jade
{
	IFileDialog* IFileDialog::s_Instance = nullptr;
	void IFileDialog::Init()
	{
		Log::Assert(s_Instance == nullptr, "IFileDialog already initialized.");
#ifdef _WIN32
		s_Instance = Win32FileDialog::Create();
#endif
	}

	IFileDialog* IFileDialog::Get()
	{
		Log::Assert(s_Instance != nullptr, "IFileDialog never initialized.");
		return s_Instance;
	}

	void IFileDialog::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}