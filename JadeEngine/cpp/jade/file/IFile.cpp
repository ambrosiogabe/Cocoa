#include "jade/file/IFile.h"
#include "jade/util/Log.h"

namespace Jade
{
	IFile* IFile::s_Instance = nullptr;

	void IFile::Init()
	{
		Log::Assert(s_Instance == nullptr, "IFile already initialized.");
#ifdef _WIN32
		s_Instance = Win32File::Create();
#endif
	}

	IFile* IFile::Get()
	{
		Log::Assert(s_Instance != nullptr, "IFile never initialized.");
		return s_Instance;
	}

	void IFile::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}