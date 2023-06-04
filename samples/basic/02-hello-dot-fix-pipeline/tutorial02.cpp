/*

        Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Tutorial 02 - Hello dot!
*/

#include <stdio.h>

#include "base/dev_gl.h"
#include "base/dev_backend.h"
#include "base/math_3d.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

GLuint VBO;

class CallBacks: public ICallbacks{
    public:
    void RenderSceneCB() override
    {
        // render
        // ------
        glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0/*index*/, 3/*size*/, GL_FLOAT/*type*/, GL_FALSE/*normalized*/, 0/*stride*/, 0/*offset*/);

        glDrawArrays(GL_POINTS, 0/*first*/, 1/*count*/);

        glDisableVertexAttribArray(0);

        OgldevBackendSwapBuffers();
    }

    void FramebufferSizeCB(int width, int height) override
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
};


static void CreateVertexBuffer()
{
    Vector3f Vertices[1];
    Vertices[0] = Vector3f(0.0f, 0.0f, 0.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
};


int main(int argc, char** argv)
{
    OgldevBackendInit(argc,argv,true,false);
    OgldevBackendCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,false,"Tutorial 02");

    CreateVertexBuffer();

    OgldevBackendRun(new CallBacks);

    OgldevBackendTerminate();

    return 0;
}
