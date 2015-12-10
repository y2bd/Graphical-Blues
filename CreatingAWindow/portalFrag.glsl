#version 330 core
out vec4 color;

in VS_OUT {
    vec3 position;
    vec2 texCoord;
} fs_in;

uniform sampler2D portalTexture;

void main()
{
    vec2 pos = gl_FragCoord.xy;
    pos.x = pos.x / 1366.0f;
    pos.y = pos.y / 768.0f;

    vec4 tex = texture(portalTexture, pos);

    color = tex;
}