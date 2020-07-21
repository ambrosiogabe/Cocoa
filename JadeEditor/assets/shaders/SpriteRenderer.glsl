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

    // Static indexing for linux based machines
    switch (int(fTexSlot)) {
        case 1:
            texColor = texture(uTextures[1], fTexCoords);
            break;
        case 2:
            texColor = texture(uTextures[2], fTexCoords);
            break;
        case 3:
            texColor = texture(uTextures[3], fTexCoords);
            break;
        case 4:
            texColor = texture(uTextures[4], fTexCoords);
            break;
        case 5:
            texColor = texture(uTextures[5], fTexCoords);
            break;
        case 6:
            texColor = texture(uTextures[6], fTexCoords);
            break;
        case 7:
            texColor = texture(uTextures[7], fTexCoords);
            break;
        case 8:
            texColor = texture(uTextures[8], fTexCoords);
            break;
        case 9:
            texColor = texture(uTextures[9], fTexCoords);
            break;
        case 10:
            texColor = texture(uTextures[10], fTexCoords);
            break;
        case 11:
            texColor = texture(uTextures[11], fTexCoords);
            break;
        case 12:
            texColor = texture(uTextures[12], fTexCoords);
            break;
        case 13:
            texColor = texture(uTextures[13], fTexCoords);
            break;
        case 14:
            texColor = texture(uTextures[14], fTexCoords);
            break;
        case 15:
            texColor = texture(uTextures[15], fTexCoords);
            break;
    }

    // If you're not on a Linux machine, you could replace the giant switch with this...
    //if (fTexSlot > 0) {
    //    int texId = int(fTexSlot);
    //    texColor = texture(uTextures[texId], fTexCoords);
    //}

    if (fTexSlot > 0) {
        color = texColor * fColor;
    } else {
        color = fColor;
    }
}