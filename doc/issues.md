1，切换glfw backend后白屏，没有渲染内容
    使用glfw需要使用glew，使用glad出现了问题

2，切换glfw backend后三角形等内容渲染不出来，但是glclearColor是生效的，并且没有报错和crash等
    需要创建VAO
    改变CullFace: glFrontFace(GL_CCW);


3, 改变cube的model矩阵diffuse光照效果不对。
    需要将position和normal等顶点属性都变换到世界空间计算光照
    Normal = vec3(model * vec4(aNormal, 1.0));