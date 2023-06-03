两种创建窗口和OpenGL的方式：

glfw+glad
freeglut+glew





该CMakeLists.txt包含了三种情况下的三方库构建：
已有的动态库        glut glew
已有的静态库        glfw
源码.c/cpp构建      glad
源码.h    构建      stb