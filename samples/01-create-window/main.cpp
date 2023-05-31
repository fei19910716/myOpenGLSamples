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

    Tutorial 01 - Create a window
*/

#include <freeglut.h>
#include <stdio.h>


static void RenderSceneCB()
{
    glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    glutSwapBuffers();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);

    int width = 800;
    int height = 600;
    glutInitWindowSize(width, height);

    int x = 200;
    int y = 100;
    glutInitWindowPosition(x, y);
    int win = glutCreateWindow("Tutorial 01");
    printf("window id: %d\n", win);

    glutDisplayFunc(RenderSceneCB);

    glutMainLoop();

    return 0;
}
