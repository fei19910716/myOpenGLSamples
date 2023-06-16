#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "base/app.h"
#include "base/technique.h"
#include "base/texture.h"
#include "base/camera.h"
#include "base/lights_common.h"
#include "base/vertices.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


class SampleApp: public App, public ICallbacks{

public:

    void RenderSceneCB() override
    {
        // per-frame time logic
        // --------------------
        frameTime = GetFrameTime();

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        // positions all containers
        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader->Enable();
        lightingShader->SetSamplerUnit("material.diffuse", 0);
        lightingShader->SetSamplerUnit("material.specular", 1);
        lightingShader->SetUniformFloat("material.shininess", 32.0f);

        lightingShader->SetUniformVec3 ("light.position",   light.Position);
        lightingShader->SetUniformVec3 ("light.direction",  light.Direction);
        lightingShader->SetUniformFloat("light.ambient",   light.AmbientIntensity);
        lightingShader->SetUniformFloat("light.diffuse",   light.DiffuseIntensity);
        lightingShader->SetUniformFloat("light.specular",  light.SpeculerIntensity);
        lightingShader->SetUniformFloat("light.constant",  light.Attenuation.Constant);
        lightingShader->SetUniformFloat("light.linear",    light.Attenuation.Linear);
        lightingShader->SetUniformFloat("light.quadratic", light.Attenuation.Exp);
        lightingShader->SetUniformFloat("light.cutOff", light.Cutoff);
        lightingShader->SetUniformFloat("light.outerCutOff", light.OuterCutOff);

        lightingShader->SetUniformVec3( "viewPos", camera->Position);


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();
        lightingShader->SetUniformMat4("projection", projection);
        lightingShader->SetUniformMat4("view", view);


        // render containers
        glBindVertexArray(VAO);

        diffuseMap->Bind(GL_TEXTURE0);
        specularMap->Bind(GL_TEXTURE1);

        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader->SetUniformMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
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
        CreateTexture();
        CreateVertexBuffer();
        CreateCamera();
        CreateLight();
    }

    ~SampleApp()
    {
        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        SAFE_DELETE(lightingShader)
        SAFE_DELETE(lightCubeShader)
        SAFE_DELETE(camera)
    }

private:

    void CreateShader(){
        lightingShader =  new Technique("shaders/5.4.lighting.vs","shaders/5.4.lighting.fs");
        lightCubeShader = new Technique("shaders/5.4.light_cube.vs","shaders/5.4.light_cube.fs");
    }

    void CreateTexture(){
        diffuseMap =  new Texture(GL_TEXTURE_2D,Utils::getAsset("textures/container2.png"));
        specularMap = new Texture(GL_TEXTURE_2D,Utils::getAsset("textures/container2_specular.png"));
    }

    void CreateCamera(){
        camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void CreateLight(){
        // lighting
        light.Position  = camera->Position;
        light.Direction = camera->Front;
        light.Color     = glm::vec3(1.0f, 1.0f, 1.0f);
        light.AmbientIntensity  = 0.2f;
        light.DiffuseIntensity  = 0.5f;
        light.SpeculerIntensity = 1.0f;

        light.Cutoff = glm::cos(glm::radians(12.5f));
        light.OuterCutOff = glm::cos(glm::radians(17.5f));

        light.Attenuation.Constant=1.0f;
        light.Attenuation.Linear = 0.1f;
        light.Attenuation.Exp = 0.03f;
        
    }


    void CreateVertexBuffer(){
        // first, configure the cube's VAO (and VBO)
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Model::CubePosNorTexVertices), Model::CubePosNorTexVertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glEnableVertexAttribArray(0);
    }

    unsigned int VAO,VBO,EBO;
    Technique   *lightingShader = nullptr,  *lightCubeShader = nullptr;
    Camera      *camera = nullptr;
    Texture     *diffuseMap = nullptr, *specularMap = nullptr;
    SpotLight light;

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

