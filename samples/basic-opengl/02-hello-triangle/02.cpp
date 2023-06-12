#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "base/window_callback.h"
#include "base/window_backend.h"
#include "base/app.h"
#include "base/utils.h"


// settings
const unsigned int SCR_WIDTH    = 800;
const unsigned int SCR_HEIGHT   = 600;
const char* vertexShaderSource  = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


class SampleApp: public App, public ICallbacks{
public:

    void RenderSceneCB() override
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 

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
        CreateVertexBuffer();
    }

    ~SampleApp()
    {
        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }

private:

    void CreateShader(){
        // build and compile our shader program
        // ------------------------------------
        // vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            DEV_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED, %s\n",infoLog);
        }
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            DEV_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED, %s\n",infoLog);
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            DEV_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED, %s\n",infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void CreateVertexBuffer(){
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left  
            0.5f, -0.5f, 0.0f, // right 
            0.0f,  0.5f, 0.0f  // top   
        }; 

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0); 
    }
    
    unsigned int VAO,VBO,shaderProgram;
};

int main(int argc, char** argv)
{
    BackendInit(argc,argv);
    BackendCreateWindow(SCR_WIDTH,SCR_HEIGHT,false/*isFullScreen*/,"Learn OpenGL");

    BackendRun(new SampleApp);

    BackendTerminate();
    return 0;
}