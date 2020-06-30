#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float texID;

out vec3 fPos;
out vec4 fColor;
out vec2 fTexCoords;
out float fTexSlot;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    fPos = aPos;
    fColor = aColor;
    fTexCoords = aTexCoords;
    fTexSlot = texID;

    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}

#type fragment
#version 330 core
// uniform float uAspect;

out vec4 color;

in vec3 fPos;
in vec4 fColor;
in vec2 fTexCoords;
in float fTexSlot;

uniform sampler2D uTextures[16];

void main()
{
    vec4 texColor = vec4(1, 1, 1, 1);
    if (fTexSlot > 0) {
        int texId = int(fTexSlot);
        texColor = texture(uTextures[texId], fTexCoords);
    }

    if (fTexSlot > 0) {
        color = texColor * fColor;
    //    if (texColor.a > 0) {
    //        color = vec4(1, 1, 1, 1);
    //    } else {
    //        color = vec4(0, 0, 0, 1);
    //    }
    } else {
        color = fColor;
    }
}