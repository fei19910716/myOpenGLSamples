#include "vulkan/vulkan.h"

#include "base/window_backend.h"
#include "base/app.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class VulkanExample: public App, public ICallbacks{
public:
    public:
    void RenderSceneCB() override
    {
        
        // render
        // ------
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        BackendSwapBuffers();
    }

    void FramebufferSizeCB(int width, int height) override
    {
        // glViewport(0, 0, width, height);
    }

    void KeyboardCB(KEY Key, KEY_STATE KeyState = KEY_STATE_PRESS) override
    {
        switch (Key)
        {
            case KEY_ESCAPE:
                BackendLeaveMainLoop();
                break;
            
            default:
                break;
        }
    }

    void CreateSwapChain(){

    }
    void CreateCommandBuffer(){

    }
    void RecordCommandBuffers(){
        
    }

private:
       
};

int main(int argc, char** argv)
{
    BackendInit(argc,argv, API_TYPE_VK);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false/*isFullScreen*/,"Learn Vulkan");

    BackendRun(new VulkanExample);

    BackendTerminate();
    return 0;
}
