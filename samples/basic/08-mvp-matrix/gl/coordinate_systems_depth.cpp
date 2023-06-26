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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(VAO);

        // bind textures on corresponding texture units
        texture1->Bind(GL_TEXTURE0);
        texture2->Bind(GL_TEXTURE1);

        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        // model: the object self transform.
        model       = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        // view: we can think that camera always looks at +Z, and locates at zero. And translating the whole scene in the reverse direction.
        view        = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
        // projection: the camera frustrum
        projection  = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // activate shader
        shaderProgram->Enable();
        shaderProgram->SetSamplerUnit("texture1", 0);
        shaderProgram->SetSamplerUnit("texture2", 1);
        shaderProgram->SetUniformMat4("model",model);
        shaderProgram->SetUniformMat4("view",view);
        shaderProgram->SetUniformMat4("projection",projection);

        
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
        shaderProgram = new GLTechnique("shaders/gl-mvp-matrix.vert","shaders/gl-mvp-matrix.frag");
    }

    void CreateTexture(){
        texture1 = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/container.jpg"));
        texture2 = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/awesomeface.png"));
    }


    void CreateVertexBuffer(){

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(MODEL::CubePosTexVertices), MODEL::CubePosTexVertices, GL_STATIC_DRAW);

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