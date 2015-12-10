#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out VS_OUT {
    vec3 position;
    vec2 texCoord;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vs_out.position = vec3(model * vec4(position, 1.0));
    vs_out.texCoord = vec2(texCoord.x, 1.0f - texCoord.y) * 50;
}