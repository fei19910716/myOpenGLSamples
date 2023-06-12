### 1 CMAKE用法
CMake可以使用add_custom_command指定在target构建的不同时机执行某些命令：

// 在${target}构建完成后执行copy操作
add_custom_command(TARGET ${target} POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/shader.vs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/shader.fs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs
    COMMENT
        "Custom command copy shader"
)

add_custom_command 还可以使用OUTPUT关键字：

// 创建一个target，ALL表示总是构建该target
add_custom_target(copy_shader ALL 
    COMMENT
        "This is generating my custom target ${target}_copy_shader"
    DEPENDS  # 该target有依赖，依赖文件不存在时会执行下面的add_custom_command
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs"
)

// 该custom_command用于生成某些文件
add_custom_command(
    OUTPUT
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.vs"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/${target}.fs"
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${target}.vs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${target}.fs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE}/shaders/
    COMMENT
        "This is generating my custom command: ${target}"
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}.vs
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}.fs
)


cmake.exe提供了一些命令可以做删除，拷贝等操作，具体可以运行cmake -E查看



### 2 VAO
glEnableVertexAttribArray(0){
    m_apiState.pCurrentVAO->EnableAttribute(index);
}

MaxAttributeVectors=16  // Max Vec4 Vertex Attributes Accessible to Vertex Shader
MaxVertexBuffers=16     // Max Vertex Buffers that can be bound to a Vertex Array Object

All the vertex attribute will be accessed by vertex shader, for this tutorial, the only attribute is position and 
it use the attribute index 0.

Also, about 16 vertex buffers can be bound to the VAO, and all the atributes are initilized to bind the buffer 0 by default.


### 3 渲染管线

#### 透视除法

Clip Space是一个顶点乘以MVP矩阵之后所在的空间，<font color=Red>Vertex Shader的输出就是在Clip Space上</font>，接着由GPU自己做<font color=Red>透视除法</font>将顶点转到NDC。

透视除法将Clip Space顶点的4个分量都除以w分量，就从Clip Space转换到了NDC了。

而NDC是一个长宽高取值范围为[-1,1]的立方体，超过这个范围的顶点，会被GPU剪裁。


#### 视口变换

想把[-1,1]的立方体的内容在屏幕上绘制出来，首先我们要把经典立方体映射到屏幕，即进行以下的过程：

1、忽略z
2、把xy平面上的[ − 1 , 1 ]转化（进行缩放和平移）成(0,width) X (0,height)（这个过程也称为视口变换）

然后就开始光栅化



### 4 矩阵变换

#### translation矩阵

m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

#### scale矩阵

m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;


#### 旋转矩阵

void Matrix4f::InitRotationX(float x)
{
    m[0][0] = 1.0f; m[0][1] = 0.0f   ;  m[0][2] = 0.0f    ; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = cosf(x);  m[1][2] = sinf(x);  m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = -sinf(x); m[2][2] = cosf(x) ; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f   ;  m[3][2] = 0.0f    ; m[3][3] = 1.0f;
}


void Matrix4f::InitRotationY(float y)
{
    m[0][0] = cosf(y); m[0][1] = 0.0f; m[0][2] = -sinf(y); m[0][3] = 0.0f;
    m[1][0] = 0.0f   ; m[1][1] = 1.0f; m[1][2] = 0.0f    ; m[1][3] = 0.0f;
    m[2][0] = sinf(y); m[2][1] = 0.0f; m[2][2] = cosf(y) ; m[2][3] = 0.0f;
    m[3][0] = 0.0f   ; m[3][1] = 0.0f; m[3][2] = 0.0f    ; m[3][3] = 1.0f;
}


void Matrix4f::InitRotationZ(float z)
{
    m[0][0] = cosf(z);  m[0][1] = sinf(z);  m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = -sinf(z); m[1][1] = cosf(z) ; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f   ;  m[2][1] = 0.0f    ; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f   ;  m[3][1] = 0.0f    ; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


void Matrix4f::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
{
    Matrix4f rx, ry, rz;

    float x = ToRadian(RotateX);
    float y = ToRadian(RotateY);
    float z = ToRadian(RotateZ);

    rx.InitRotationX(x);
    ry.InitRotationY(y);
    rz.InitRotationZ(z);

    // InitRotateTransform
    *this = rz * ry * rx;

    // InitRotateTransformZYX
    *this = rx * ry * rz;
}



void Matrix4f::InitRotateTransform(const Quaternion& quat)
{
    float yy2 = 2.0f * quat.y * quat.y;
    float xy2 = 2.0f * quat.x * quat.y;
    float xz2 = 2.0f * quat.x * quat.z;
    float yz2 = 2.0f * quat.y * quat.z;
    float zz2 = 2.0f * quat.z * quat.z;
    float wz2 = 2.0f * quat.w * quat.z;
    float wy2 = 2.0f * quat.w * quat.y;
    float wx2 = 2.0f * quat.w * quat.x;
    float xx2 = 2.0f * quat.x * quat.x;
    m[0][0] = - yy2 - zz2 + 1.0f;
    m[0][1] = xy2 + wz2;
    m[0][2] = xz2 - wy2;
    m[0][3] = 0;
    m[1][0] = xy2 - wz2;
    m[1][1] = - xx2 - zz2 + 1.0f;
    m[1][2] = yz2 + wx2;
    m[1][3] = 0;
    m[2][0] = xz2 + wy2;
    m[2][1] = yz2 - wx2;
    m[2][2] = - xx2 - yy2 + 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = m[3][1] = m[3][2] = 0;
    m[3][3] = 1.0f;
}

#### 透视矩阵

void Matrix4f::InitPersProjTransform(const PersProjInfo& p)
{
    float ar         = p.Height / p.Width;
    float zRange     = p.zNear - p.zFar;
    float tanHalfFOV = tanf(ToRadian(p.FOV / 2.0f));

    m[0][0] = 1/tanHalfFOV; m[0][1] = 0.0f;                 m[0][2] = 0.0f;                        m[0][3] = 0.0;
    m[1][0] = 0.0f;         m[1][1] = 1.0f/(tanHalfFOV*ar); m[1][2] = 0.0f;                        m[1][3] = 0.0;
    m[2][0] = 0.0f;         m[2][1] = 0.0f;                 m[2][2] = (-p.zNear - p.zFar)/zRange ; m[2][3] = 2.0f*p.zFar*p.zNear/zRange;
    m[3][0] = 0.0f;         m[3][1] = 0.0f;                 m[3][2] = 1.0f;                        m[3][3] = 0.0;
}

#### 正交矩阵

void Matrix4f::InitOrthoProjTransform(const OrthoProjInfo& p)
{
    float l = p.l;
    float r = p.r;
    float b = p.b;
    float t = p.t;
    float n = p.n;
    float f = p.f;

    m[0][0] = 2.0f/(r - l); m[0][1] = 0.0f;         m[0][2] = 0.0f;         m[0][3] = -(r + l)/(r - l);
    m[1][0] = 0.0f;         m[1][1] = 2.0f/(t - b); m[1][2] = 0.0f;         m[1][3] = -(t + b)/(t - b);
    m[2][0] = 0.0f;         m[2][1] = 0.0f;         m[2][2] = 2.0f/(f - n); m[2][3] = -(f + n)/(f - n);
    m[3][0] = 0.0f;         m[3][1] = 0.0f;         m[3][2] = 0.0f;         m[3][3] = 1.0;
}


#### 相机LookAt矩阵

// 相机位于原点
void Matrix4f::InitCameraTransform(const Vector3f& Target, const Vector3f& Up)
{
    Vector3f N = Target;
    N.Normalize();

    Vector3f UpNorm = Up;
    UpNorm.Normalize();

    Vector3f U;
    U = UpNorm.Cross(N);
    U.Normalize();

    Vector3f V = N.Cross(U);

    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
}

// 相机位于Pos
void Matrix4f::InitCameraTransform(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    Matrix4f CameraTranslation;
    CameraTranslation.InitTranslationTransform(-Pos.x, -Pos.y, -Pos.z);

    Matrix4f CameraRotateTrans;
    CameraRotateTrans.InitCameraTransform(Target, Up);

    *this = CameraRotateTrans * CameraTranslation;
}


### 5 Texture
对于target GL_TEXTURE_2D来说，其有一定数量的texture unit，shader和cpu通过texture unit来进行bind。

// 指定shader中的sampler对应的unit
gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
glUniform1i(gSampler, 1);

// 将纹理数据绑定到unit
pTexture->Bind(GL_TEXTURE1);