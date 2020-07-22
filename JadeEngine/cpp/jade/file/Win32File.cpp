#include "jade/file/IFile.h"

namespace Jade
{
    File* Win32File::ImplOpenFile(const char* filename)
    {
        File* file = new File();
        file->m_Filename = filename;

        std::ifstream inputStream(filename);

        inputStream.seekg(0, std::ios::end);
        file->m_Size = inputStream.tellg();
        file->m_Data.reserve(file->m_Size);
        inputStream.seekg(0, std::ios::beg);

        file->m_Data.assign((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());

        return file;
	}

    void Win32File::ImplCloseFile(File* file)
    {
        delete file;
    }

    bool Win32File::ImplWriteFile(const char* data, const char* filename)
    {
        std::ofstream outStream(filename);
        outStream << data;
        outStream.close();
        return true;
    }
}