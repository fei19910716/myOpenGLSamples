
#pragma once

#include <cassert>

#include "types.h"
#include "window_callback.h"

enum BACKEND_TYPE {
    BACKEND_TYPE_GLFW
};


void BackendInit(int argc, char** argv);

void BackendTerminate();

bool BackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle);

void BackendRun(ICallbacks* pCallbacks);

void BackendLeaveMainLoop();

void BackendSwapBuffers();

void BackendSetMousePos(uint x, uint y);

KEY BackendKeyToKey(uint Key);
