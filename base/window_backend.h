
#pragma once

#include <cassert>

#include "types.h"
#include "window_callback.h"

enum BACKEND_TYPE {
    BACKEND_TYPE_GLFW
};

enum API_TYPE {
    API_TYPE_GL,
    API_TYPE_VK
};


void  BackendInit(int argc, char** argv, API_TYPE type = API_TYPE::API_TYPE_GL);

void  BackendTerminate();

bool  BackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle);

void  BackendRun(ICallbacks* pCallbacks);

void  BackendLeaveMainLoop();

void  BackendSwapBuffers();

void  BackendSetMousePos(uint x, uint y);

void* BackendWindowHandle();

KEY  BackendKeyToKey(uint Key);
