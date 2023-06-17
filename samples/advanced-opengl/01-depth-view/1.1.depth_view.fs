#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far = 100.0; 
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far to get depth in range [0,1] for visualization purposes
    FragColor = vec4(vec3(depth), 1.0);
}

/*
 * 这个shader用于显示场景的深度图，在NDC坐标中Z左边是线性的，但是经过projection变换，Z周边不是线性的，显示的时候将其转换到NDC坐标中，不然场景会是黑色的（Z坐标太小）。
*/