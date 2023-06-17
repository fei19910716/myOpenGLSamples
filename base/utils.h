
#pragma once

#ifndef _WIN64
    #include <unistd.h>
#endif

#include <stdio.h>
#include <string>

#include "base/types.h"
#include "base/log_system.h"

#ifdef _WIN64
    #define SNPRINTF _snprintf_s
    #define VSNPRINTF vsnprintf_s
    #define RANDOM rand
    #define SRANDOM srand((unsigned)time(NULL))
    #pragma warning (disable: 4566)
#else
    #define SNPRINTF snprintf
    #define VSNPRINTF vsnprintf
    #define RANDOM random
    #define SRANDOM srandom(getpid())
#endif

namespace UTILS{

std::string getAsset(const std::string& path);
bool        ReadFile(const char* fileName, std::string& outFile);
long long   GetCurrentTimeMillis();
void        DevError(const char* pFileName, uint line, const char* msg, ... );
void        DevInfo (const char* pFileName, uint line, const char* msg, ... );
}

#define DEV_ERROR(msg, ...)         UTILS::DevError(__FILE__, __LINE__, msg, __VA_ARGS__)
#define DEV_INFO(msg, ...)          UTILS::DevInfo (__FILE__, __LINE__, msg, __VA_ARGS__)
#define ZERO_MEM(a)                 memset(a, 0, sizeof(a))
#define GLCheckError()              (glGetError() == GL_NO_ERROR)
#define INVALID_UNIFORM_LOCATION    0xffffffff
#define INVALID_OGL_VALUE           0xffffffff
#define ARRAY_SIZE_IN_ELEMENTS(a)   (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p)              if (p) { delete p; p = NULL; }