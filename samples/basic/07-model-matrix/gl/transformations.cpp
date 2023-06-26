#include <glad/glad.h>
#include <GLFW/glfw3.h>


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
        glBindVertexArray(VAO);

        // bind textures on corresponding texture units
        texture1->Bind(GL_TEXTURE0);
        texture2->Bind(GL_TEXTURE1);

        // create transformations
        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        shaderProgram->Enable();
        shaderProgram->SetSamplerUnit("texture1", 0);
        shaderProgram->SetSamplerUnit("texture2", 1);
        shaderProgram->SetUniformMat4("transform",transform);

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
        glDeleteBuffers(1, &EBO);
        SAFE_DELETE(shaderProgram)
        SAFE_DELETE(texture1)
        SAFE_DELETE(texture2)
    }

private:

    void CreateShader(){
        shaderProgram = new GLTechnique("shaders/gl-model-matrix.vert","shaders/gl-model-matrix.frag");
    }

    void CreateTexture(){
        texture1 = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/container.jpg"));
        texture2 = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/awesomeface.png"));
    }


    void CreateVertexBuffer(){

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(MODEL::QuadPosTexVertices), MODEL::QuadPosTexVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MODEL::QuadIndices), MODEL::QuadIndices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    unsigned int VAO,VBO,EBO;
    GLTechnique* shaderProgram = nullptr;
    GLTexture *texture1 = nullptr, *texture2 = nullptr;

};

int main(int argc, char** argv)
{
    BackendInit(argc,argv);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false,"LearnOpenGL");
    
    BackendRun(new SampleApp);

    BackendTerminate();

    return 0;
}