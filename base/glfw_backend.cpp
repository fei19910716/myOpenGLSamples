

#ifdef WIN32
#include <Windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <stdio.h>

#include "utils.h"
#include "glfw_backend.h"

static ICallbacks* s_glfw_pCallbacks = NULL;
static GLFWwindow* s_pWindow         = NULL;

static MOUSE GLFWMouseToMouse(uint Button)
{
        switch (Button) {
        case GLFW_MOUSE_BUTTON_LEFT:
                return MOUSE_BUTTON_LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT:
                return MOUSE_BUTTON_RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE:
                return MOUSE_BUTTON_MIDDLE;
        default:
                DEV_ERROR("Unimplemented  mouse button %d\n", Button);
        }

        return MOUSE_UNDEFINED;
}

static void KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    KEY Key = GLFWKeyToKey(key);
    KEY_STATE KeyState = (action == GLFW_PRESS) ? KEY_STATE_PRESS : KEY_STATE_RELEASE;
    s_glfw_pCallbacks->KeyboardCB(Key, KeyState);
}


static void CursorPosCallback(GLFWwindow* pWindow, double x, double y)
{
    s_glfw_pCallbacks->MousePassiveCB(x, y);
}


static void MouseButtonCallback(GLFWwindow* pWindow, int Button, int Action, int Mode)
{
    MOUSE Mouse = GLFWMouseToMouse(Button);

    KEY_STATE State = (Action == GLFW_PRESS) ? KEY_STATE_PRESS : KEY_STATE_RELEASE;

    double x, y;

    glfwGetCursorPos(pWindow, &x, &y);

    s_glfw_pCallbacks->MousePressCB(Mouse, State, x, y);
}

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    s_glfw_pCallbacks->FramebufferSizeCB(width,height);
}

static void ScrollCallback(GLFWwindow* window, double x, double y)
{
    s_glfw_pCallbacks->ScrollCallback(x,y);
}

static void glfwInitCallbacks()
{
    glfwSetKeyCallback(s_pWindow, KeyCallback);
    glfwSetCursorPosCallback(s_pWindow, CursorPosCallback);
    glfwSetMouseButtonCallback(s_pWindow, MouseButtonCallback);
    glfwSetFramebufferSizeCallback(s_pWindow, FramebufferSizeCallback);
    glfwSetScrollCallback(s_pWindow, ScrollCallback);
}

void GLFWErrorCallback(int error, const char* description)
{
#ifdef WIN32
    char msg[1000];
    _snprintf_s(msg, sizeof(msg), "GLFW error %d - %s", error, description);
    MessageBoxA(NULL, msg, NULL, 0);
#else
    fprintf(stderr, "GLFW error %d - %s", error, description);
#endif
    exit(0);
}


void GLFWBackendInit(int argc, char** argv)
{
    glfwSetErrorCallback(GLFWErrorCallback);

    if (glfwInit() != 1) {
        DEV_ERROR("Error initializing GLFW");
        exit(1);
    }

    int Major, Minor, Rev;

    glfwGetVersion(&Major, &Minor, &Rev);

    printf("GLFW %d.%d.%d initialized\n", Major, Minor, Rev);
}


void GLFWBackendTerminate()
{
    glfwDestroyWindow(s_pWindow);
    glfwTerminate();
}


bool GLFWBackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle, bool isVulkan)
{
    GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;

    if(isVulkan == true)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
    else
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif
    }

    s_pWindow = glfwCreateWindow(Width, Height, pTitle, pMonitor, NULL);

    if (s_pWindow == nullptr) {
        DEV_ERROR("error creating window");
        exit(1);
    }

    if(isVulkan == false) {
        glfwMakeContextCurrent(s_pWindow);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            DEV_ERROR("Failed to initialize GLAD");
            exit(1);
        }
    }

    return (s_pWindow != NULL);
}

void GLFWBackendRun(ICallbacks* pCallbacks, bool isVulkan)
{
    if (!pCallbacks) {
        DEV_ERROR("callbacks not specified");
        exit(1);
    }

    s_glfw_pCallbacks = pCallbacks;
    glfwInitCallbacks();

    while (!glfwWindowShouldClose(s_pWindow)) {
        s_glfw_pCallbacks->RenderSceneCB();
        if(!isVulkan) glfwSwapBuffers(s_pWindow);
        glfwPollEvents();
    }
}


void GLFWBackendSwapBuffers()
{
    // Nothing to do here
}


void GLFWBackendLeaveMainLoop()
{
    glfwSetWindowShouldClose(s_pWindow, 1);
}


void GLFWBackendSetMousePos(uint x, uint y)
{
    glfwSetCursorPos(s_pWindow, (double)x, (double)y);
}


void* GLFWBackendWindowHandle(){
    return reinterpret_cast<void*>(glfwGetWin32Window(s_pWindow));
}

KEY GLFWKeyToKey(uint Key)
{
    if (Key >= GLFW_KEY_SPACE && Key <= GLFW_KEY_RIGHT_BRACKET) {
        return (KEY)Key;
    }

    switch (Key) {
        case GLFW_KEY_ESCAPE:
            return KEY_ESCAPE;
        case GLFW_KEY_ENTER:
            return KEY_ENTER;
        case GLFW_KEY_TAB:
            return KEY_TAB;
        case GLFW_KEY_BACKSPACE:
            return KEY_BACKSPACE;
        case GLFW_KEY_INSERT:
            return KEY_INSERT;
        case GLFW_KEY_DELETE:
            return KEY_DELETE;
        case GLFW_KEY_RIGHT:
            return KEY_RIGHT;
        case GLFW_KEY_LEFT:
            return KEY_LEFT;
        case GLFW_KEY_DOWN:
            return KEY_DOWN;
        case GLFW_KEY_UP:
            return KEY_UP;
        case GLFW_KEY_PAGE_UP:
            return KEY_PAGE_UP;
        case GLFW_KEY_PAGE_DOWN:
            return KEY_PAGE_DOWN;
        case GLFW_KEY_HOME:
            return KEY_HOME;
        case GLFW_KEY_END:
            return KEY_END;
        case GLFW_KEY_F1:
            return KEY_F1;
        case GLFW_KEY_F2:
            return KEY_F2;
        case GLFW_KEY_F3:
            return KEY_F3;
        case GLFW_KEY_F4:
            return KEY_F4;
        case GLFW_KEY_F5:
            return KEY_F5;
        case GLFW_KEY_F6:
            return KEY_F6;
        case GLFW_KEY_F7:
            return KEY_F7;
        case GLFW_KEY_F8:
            return KEY_F8;
        case GLFW_KEY_F9:
            return KEY_F9;
        case GLFW_KEY_F10:
            return KEY_F10;
        case GLFW_KEY_F11:
            return KEY_F11;
        case GLFW_KEY_F12:
            return KEY_F12;
        default:
            DEV_ERROR("Unimplemented  key %d\n", Key);
    }

    return KEY_UNDEFINED;
}

