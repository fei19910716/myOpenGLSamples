#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "base/app.h"
#include "base/gltechnique.h"
#include "base/gltexture.h"
#include "base/camera.h"
#include "base/glmesh.h"

#include <map>

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

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw objects
        shader->Enable();
        glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shader->SetUniformMat4("projection", projection);
        shader->SetUniformMat4("view", view);
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
        model = glm::mat4(1.0f);
        shader->SetUniformMat4("model", model);
        planeMesh->Draw(shader);
        // windows (from furthest to nearest)
        // sort the transparent windows before rendering
        // ---------------------------------------------
        vector<glm::vec3> windows
        {
            glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3( 1.5f, 0.0f, 0.51f),
            glm::vec3( 0.0f, 0.0f, 0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3( 0.5f, 0.0f, -0.6f)
        };
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera->Position - windows[i]);
            sorted[distance] = windows[i];
        }
        
        windowTexture->Bind(GL_TEXTURE0);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader->SetUniformMat4("model", model);
            windowMesh->Draw(shader);
        }

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
        shader = new GLTechnique("shaders/3.2.blending.vs","shaders/3.2.blending.fs");
    }

    void CreateCamera(){
        camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void CreateTextures(){
        cubeTexture = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/marble.jpg"));
        floorTexture = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/metal.png"));
        windowTexture = new GLTexture(GL_TEXTURE_2D,UTILS::getAsset("textures/window.png"));
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

        // grass VAO

        float windowVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
        };

        auto windowVBO = GLVertexBuffer::builder().Attribute(AttributeType::POSITION, 0, 3, AttributeDataType::GLFLOAT, false, 5 * sizeof(float), 0)
                                                 .Attribute(AttributeType::TexCoord, 1, 2, AttributeDataType::GLFLOAT, false, 5 * sizeof(float), 3 * sizeof(float))
                                                 .Buffer(&windowVertices)
                                                 .Size(sizeof(windowVertices))
                                                 .VertexCount(6)
                                                 .build();
        windowMesh = new GLMesh(GLVertexArray::builder().VBO(windowVBO).build());

    }

    GLMesh      *cubeMesh = nullptr, *planeMesh = nullptr, *windowMesh = nullptr;
    GLTechnique *shader = nullptr;
    Camera      *camera = nullptr;
    GLTexture   *cubeTexture = nullptr, *floorTexture = nullptr, *windowTexture = nullptr;

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

