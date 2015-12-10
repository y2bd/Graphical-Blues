#version 330 core
out vec4 color;

uniform bool colliding;

void main()
{             
    if (colliding) color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}  