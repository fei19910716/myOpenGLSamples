
#pragma once

#include "types.h"
#include "window_callback.h"

void GLFWBackendInit(int argc, char** argv);

void GLFWBackendTerminate();

bool GLFWBackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle, bool isVulkan = false);

void GLFWBackendRun(ICallbacks* pCallbacks, bool isVulkan = false);

void GLFWBackendSwapBuffers();

void GLFWBackendLeaveMainLoop();

void GLFWBackendSetMousePos(uint x, uint y);

void* GLFWBackendWindowHandle();

KEY GLFWKeyToKey(uint Key);
