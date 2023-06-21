
#include "utils.h"
#include "window_backend.h"
#include "glfw_backend.h"

static BACKEND_TYPE sBackendType = BACKEND_TYPE_GLFW;
static API_TYPE sAPIType = API_TYPE_GL;

void BackendInit(int argc, char** argv, API_TYPE type)
{
    sAPIType = type;
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            GLFWBackendInit(argc, argv);
            break;
        default:
            assert(0);
    }
}


void BackendTerminate()
{
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            GLFWBackendTerminate();
            break;
        default:
            assert(0);
    }
}


bool BackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle)
{
    //TwWindowSize(Width, Height);

    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            return GLFWBackendCreateWindow(Width, Height, isFullScreen, pTitle, sAPIType == API_TYPE_VK);
        default:
            assert(0);
    }

    return false;
}


void BackendRun(ICallbacks* pCallbacks)
{
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            GLFWBackendRun(pCallbacks, sAPIType == API_TYPE_VK);
            break;
        default:
            assert(0);
    }
}


void BackendSwapBuffers()
{
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            GLFWBackendSwapBuffers();
            break;
        default:
            assert(0);
    }
}


void BackendLeaveMainLoop()
{
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            GLFWBackendLeaveMainLoop();
            break;
        default:
            assert(0);
    }
}


void BackendSetMousePos(uint x, uint y)
{
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            GLFWBackendSetMousePos(x, y);
            break;
        default:
            assert(0);
    }
}

void* BackendWindowHandle(){
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            return GLFWBackendWindowHandle();
            break;
        default:
            return nullptr;
    }
}


KEY BackendKeyToKey(uint Key)
{
    switch (sBackendType) {
        case BACKEND_TYPE_GLFW:
            return GLFWKeyToKey(Key);
        default:
            return KEY_UNDEFINED;
    }
}
