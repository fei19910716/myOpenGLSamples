
#include "base/app.h"
#include "base/gl/gltechnique.h"
#include "base/gl/gltexture.h"
#include "base/vertices.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


class SampleApp: public App, public ICallbacks{

public:

    void RenderSceneCB() override
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        // render container
        shaderProgram->Enable();
        texture->Bind(GL_TEXTURE0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        BackendSwapBuffers();
    }

    void FramebufferSizeCB(int width, int height) override
    {
        glViewport(0, 0, width, height);
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


    SampleApp()
    {
        CreateShader();
        CreateTexture();
        CreateVertexBuffer();
    }

    ~SampleApp()
    {
        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        SAFE_DELETE(shaderProgram)
    }

private:

    void CreateShader(){
        shaderProgram = new GLTechnique("shaders/gl-texture.vert","shaders/gl-texture.frag");
    }

    void CreateTexture(){
        texture = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/container.jpg"));
    }


    void CreateVertexBuffer(){
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(MODEL::QuadPosColTexVertices), MODEL::QuadPosColTexVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MODEL::QuadIndices), MODEL::QuadIndices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    unsigned int VAO,VBO,EBO;
    GLTechnique* shaderProgram = nullptr;
    GLTexture* texture = nullptr;

};

int main(int argc, char** argv)
{
    BackendInit(argc,argv);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false,"LearnOpenGL");
    
    BackendRun(new SampleApp);

    BackendTerminate();

    return 0;
}
