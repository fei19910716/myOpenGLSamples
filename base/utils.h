
#pragma once

#ifndef _WIN64
    #include <unistd.h>
#endif

#include <stdio.h>
#include <string>

#include "types.h"

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



bool        ReadFile(const char* fileName, std::string& outFile);
long long   GetCurrentTimeMillis();
void        FileError(const char* pFileName, uint line, const char* pErrorMsg);
void        DevError(const char* pFileName, uint line, const char* msg, ... );


#define DEV_ERROR(msg, ...)         DevError(__FILE__, __LINE__, msg, __VA_ARGS__)
#define FILE_ERROR(ErrorMsg)        FileError(__FILE__, __LINE__, ErrorMsg);
#define ZERO_MEM(a)                 memset(a, 0, sizeof(a))
#define GLCheckError()              (glGetError() == GL_NO_ERROR)
#define INVALID_UNIFORM_LOCATION    0xffffffff
#define INVALID_OGL_VALUE           0xffffffff
#define ARRAY_SIZE_IN_ELEMENTS(a)   (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p)              if (p) { delete p; p = NULL; }