#include "externalLibs.h"

#include "jade/util/Log.h"
#include "jade/core/Application.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace Jade {
    void Log::_Log(const char* filename, int line, const char* format, ...)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
        printf("%s (line %d) Log: \n", filename, line);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
        printf("-> ");

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        printf("\n");
    }

    void Log::_Info(const char* filename, int line, const char* format, ...) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
        printf("%s (line %d) Info: \n", filename, line);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
        printf("-> ");

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        printf("\n");
    }

    void Log::_Warning(const char* filename, int line, const char* format, ...) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
        printf("%s (line %d) Warning: \n", filename, line);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
        printf("-> ");

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        printf("\n");
    }

    void Log::_Error(const char* filename, int line, const char* format, ...) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
        printf("%s (line %d) Error: \n", filename, line);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
        printf("-> ");

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        printf("\n");
    }

    void Log::_Assert(const char* filename, int line, int condition, const char* format, ...) {
        if (!condition) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
            printf("%s (line %d) Assertion Failure: \n", filename, line);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
            printf("-> ");

            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);

            printf("\n");

            if (Application::Get() != nullptr)
            {
                Application::Get()->GetWindow()->Destroy();
            }
        }   
    }
}