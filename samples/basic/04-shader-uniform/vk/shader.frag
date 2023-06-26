#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(push_constant, std430) uniform PushConsts {
	vec4 color;
} pushConsts;

void main() {
    outColor = vec4(pushConsts.color);
}
