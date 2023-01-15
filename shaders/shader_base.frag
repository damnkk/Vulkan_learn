#version 450
layout(push_constant) uniform ConstantData{
    uint index;
    int num;
}constantData;

layout(binding = 1) uniform sampler2D texSampler[7];
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() {
    outColor = texture(texSampler[constantData.index], fragTexCoord);
}