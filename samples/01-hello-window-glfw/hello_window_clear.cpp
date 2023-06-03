
#include <iostream>

#define BACKEND_TYPE_GLFW
#include "base/dev_backend.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


class CallBacks: public ICallbacks{
    public:
    void RenderSceneCB() override
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE OgldevKeyState = OGLDEV_KEY_STATE_PRESS) override {
        switch (OgldevKey)
        {
        case OGLDEV_KEY_ESCAPE:
            OgldevBackendLeaveMainLoop();
            break;
        
        default:
            break;
        }
    }

    void FramebufferSizeCB(int width, int height) override
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
};

int main(int argc, char** argv)
{
    OgldevBackendInit(OGLDEV_BACKEND_TYPE_GLFW,argc,argv,true,false);
    OgldevBackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false,"LearnOpenGL");

    OgldevBackendRun(new CallBacks);

    OgldevBackendTerminate();

    return 0;
}