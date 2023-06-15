
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include <iostream>
#include <fstream>

#include "utils.h"
#include "root_directory.h"

std::string Utils::getAsset(const std::string& path){
    static char const * givenRoot = logl_root;
    static std::string root = (givenRoot != nullptr ? givenRoot : std::string());

    return root + std::string("/data/") + path;
}


bool Utils::ReadFile(const char* pFileName, std::string& outFile)
{
    std::ifstream f(pFileName);

    bool ret = false;

    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            outFile.append(line);
            outFile.append("\n");
        }

        f.close();

        ret = true;
    }
    else {
        DEV_ERROR("unable to open file %s",pFileName);
    }

    return ret;
}


void Utils::DevError(const char* pFileName, uint line, const char* format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    VSNPRINTF(msg, sizeof(msg), format, args);
    va_end(args);

#ifdef _WIN32
    char msg2[1000];
    _snprintf_s(msg2, sizeof(msg2), "%s:%d: %s", pFileName, line, msg);
    MessageBoxA(NULL, msg2, NULL, 0);
#else
    fprintf(stderr, "%s:%d - %s", pFileName, line, msg);
#endif
}


long long Utils::GetCurrentTimeMillis()
{
#ifdef _WIN32
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif
}