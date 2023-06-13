#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "base/window_backend.h"
#include "base/app.h"
#include "base/technique.h"
#include "base/texture.h"


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
        shaderProgram = new Technique("shaders/06-texture.vs","shaders/06-texture.fs");
    }

    void CreateTexture(){
        texture = new Texture(GL_TEXTURE_2D,"textures/container.jpg");
    }


    void CreateVertexBuffer(){
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
            // positions          // colors           // texture coords
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
        };
        unsigned int indices[] = {  
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
    Technique* shaderProgram = nullptr;
    Texture* texture = nullptr;

};

int main(int argc, char** argv)
{
    BackendInit(argc,argv);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false,"LearnOpenGL");
    
    BackendRun(new SampleApp);

    BackendTerminate();

    return 0;
}
