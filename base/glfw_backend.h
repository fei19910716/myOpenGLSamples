
#pragma once

#include "types.h"
#include "window_callback.h"

void GLFWBackendInit(int argc, char** argv);

void GLFWBackendTerminate();

bool GLFWBackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle);

void GLFWBackendRun(ICallbacks* pCallbacks);

void GLFWBackendSwapBuffers();

void GLFWBackendLeaveMainLoop();

void GLFWBackendSetMousePos(uint x, uint y);

KEY GLFWKeyToKey(uint Key);
