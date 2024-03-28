#version 400
out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D diffuseTexture;
uniform vec3 colorPickColor;
void main()
{
    FragColor = vec4(colorPickColor, 1.0f);
}
