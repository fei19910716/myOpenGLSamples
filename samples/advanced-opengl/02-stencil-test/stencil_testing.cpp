#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "base/app.h"
#include "base/gltechnique.h"
#include "base/gltexture.h"
#include "base/camera.h"
#include "base/glmesh.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

class SampleApp: public App, public ICallbacks{

public:

    void RenderSceneCB() override
    {
        frameTime = GetFrameTime();

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

        // configure global opengl state
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        // set uniforms
        shaderSingleColor->Enable();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderSingleColor->SetUniformMat4("view", view);
        shaderSingleColor->SetUniformMat4("projection", projection);

        shader->Enable();
        shader->SetUniformMat4("view", view);
        shader->SetUniformMat4("projection", projection);

        // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. 
        // We set its mask to 0x00 to not write to the stencil buffer.
        glStencilMask(0x00);
        // floor
        floorTexture->Bind(GL_TEXTURE0);
        shader->SetUniformMat4("model", glm::mat4(1.0f));
        planeMesh->Draw(shader);

        // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // --------------------------------------------------------------------
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // cubes
        cubeTexture->Bind(GL_TEXTURE0);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader->SetUniformMat4("model", model);
        cubeMesh->Draw(shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader->SetUniformMat4("model", model);
        cubeMesh->Draw(shader);

        // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
        // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
        // the objects' size differences, making it look like borders.
        // -----------------------------------------------------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        
        float scale = 1.1f;
        // cubes
        shaderSingleColor->Enable();
        cubeTexture->Bind(GL_TEXTURE0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor->SetUniformMat4("model", model);
        cubeMesh->Draw(shaderSingleColor);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor->SetUniformMat4("model", model);
        cubeMesh->Draw(shaderSingleColor);

        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

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
                camera->ProcessKeyboard(Key,frameTime);
        }
    }

    void MousePassiveCB(double x, double y) override 
    {
        float xpos = static_cast<float>(x);
        float ypos = static_cast<float>(y);

        // std::cout << "(pos:" << x << "," << y << ")" << std::endl;

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera->ProcessMouseMovement(xoffset, yoffset);
    }

    void ScrollCallback(double x, double y) override
    {
        camera->ProcessMouseScroll(static_cast<float>(y));
    }


    SampleApp()
    {
        CreateShader();
        CreateVertexBuffer();
        CreateCamera();
        CreateTextures();
    }

    ~SampleApp()
    {
        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        SAFE_DELETE(shader)
        SAFE_DELETE(camera)
    }

private:

    void CreateShader(){
        shader = new GLTechnique("shaders/2.stencil_testing.vs","shaders/2.stencil_testing.fs");
        shaderSingleColor = new GLTechnique("shaders/2.stencil_testing.vs","shaders/2.stencil_single_color.fs");
    }

    void CreateCamera(){
        camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void CreateTextures(){
        cubeTexture = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/marble.jpg"));
        floorTexture = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/metal.png"));
    }


    void CreateVertexBuffer(){
        

        // cube VAO
        auto cubeVBO = GLVertexBuffer::builder().Attribute(AttributeType::POSITION, 0, 3, AttributeDataType::GLFLOAT, false, 5 * sizeof(float), 0)
                                                .Attribute(AttributeType::TexCoord, 1, 2, AttributeDataType::GLFLOAT, false, 5 * sizeof(float), 3 * sizeof(float))
                                                .Buffer(&MODEL::CubePosTexVertices)
                                                .Size(sizeof(MODEL::CubePosTexVertices))
                                                .VertexCount(36)
                                                .build();
        cubeMesh = new GLMesh(GLVertexArray::builder().VBO(cubeVBO).build());

        // plane VAO
        float planeVertices[] = {
            // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
            5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
        };

        auto planeVBO = GLVertexBuffer::builder().Attribute(AttributeType::POSITION, 0, 3, AttributeDataType::GLFLOAT, false, 5 * sizeof(float), 0)
                                                 .Attribute(AttributeType::TexCoord, 1, 2, AttributeDataType::GLFLOAT, false, 5 * sizeof(float), 3 * sizeof(float))
                                                 .Buffer(&planeVertices)
                                                 .Size(sizeof(planeVertices))
                                                 .VertexCount(6)
                                                 .build();
        planeMesh = new GLMesh(GLVertexArray::builder().VBO(planeVBO).build());
    }

    GLMesh      *cubeMesh = nullptr, *planeMesh = nullptr;
    GLTechnique *shader = nullptr, *shaderSingleColor = nullptr;
    Camera      *camera = nullptr;
    GLTexture   *cubeTexture = nullptr, *floorTexture = nullptr;

    float frameTime;

};

int main(int argc, char** argv)
{
    BackendInit(argc,argv);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false,"LearnOpenGL");

    BackendRun(new SampleApp);

    BackendTerminate();

    return 0;
}
