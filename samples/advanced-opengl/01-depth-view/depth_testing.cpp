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
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;


class SampleApp: public App, public ICallbacks{

public:

    void RenderSceneCB() override
    {
        frameTime = GetFrameTime();

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS); 

        shader->Enable();
        shader->SetSamplerUnit("texture1", 0);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader->SetUniformMat4("view", view);
        shader->SetUniformMat4("projection", projection);
        // cubes
        cubeTexture->Bind(GL_TEXTURE0);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader->SetUniformMat4("model", model);
        cubeMesh->Draw(shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader->SetUniformMat4("model", model);
        cubeMesh->Draw(shader);
        // floor
        floorTexture->Bind(GL_TEXTURE0);
        shader->SetUniformMat4("model", glm::mat4(1.0f));
        planeMesh->Draw(shader);

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
        shader = new GLTechnique("shaders/1.1.depth_view.vs","shaders/1.1.depth_view.fs");
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
    GLTechnique *shader = nullptr;
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
