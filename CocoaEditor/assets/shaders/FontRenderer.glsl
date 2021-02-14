#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float texID;
layout (location = 4) in uint entityID;

out vec3 fPos;
out vec4 fColor;
out vec2 fTexCoords;
out float fTexSlot;
flat out uint fEntityID;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    fPos = aPos;
    fColor = aColor;
    fTexCoords = aTexCoords;
    fTexSlot = texID;
    fEntityID = entityID;

    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}

#type fragment
#version 330 core
layout (location = 0) out vec4 color;
layout (location = 1) out uint entityID;

in vec3 fPos;
in vec4 fColor;
in vec2 fTexCoords;
in float fTexSlot;
flat in uint fEntityID;

uniform sampler2D uTextures[16];
uniform uint uActiveEntityID;

const float offset = 1.0 / 300.0;
const float weight = 0.06;

void main()
{
    vec4 texColor = vec4(1, 1, 1, 1);
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right 
    );

    float kernel[9] = float[](
        weight, weight, weight, 
        weight, weight, weight, 
        weight, weight, weight
    );

    float sampleTex[9];
    // Static indexing for linux based machines
    switch (int(fTexSlot)) {
        case 1:
            texColor = texture(uTextures[1], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[1], fTexCoords + offsets[i]).a;
            }
            break;
        case 2:
            texColor = texture(uTextures[2], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[2], fTexCoords + offsets[i]).a;
            }
            break;
        case 3:
            texColor = texture(uTextures[3], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[3], fTexCoords + offsets[i]).a;
            }
            break;
        case 4:
            texColor = texture(uTextures[4], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[4], fTexCoords + offsets[i]).a;
            }
            break;
        case 5:
            texColor = texture(uTextures[5], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[5], fTexCoords + offsets[i]).a;
            }
            break;
        case 6:
            texColor = texture(uTextures[6], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[6], fTexCoords + offsets[i]).a;
            }
            break;
        case 7:
            texColor = texture(uTextures[7], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[7], fTexCoords + offsets[i]).a;
            }
            break;
        case 8:
            texColor = texture(uTextures[8], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[8], fTexCoords + offsets[i]).a;
            }
            break;
        case 9:
            texColor = texture(uTextures[9], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[9], fTexCoords + offsets[i]).a;
            }
            break;
        case 10:
            texColor = texture(uTextures[10], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[10], fTexCoords + offsets[i]).a;
            }
            break;
        case 11:
            texColor = texture(uTextures[11], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[11], fTexCoords + offsets[i]).a;
            }
            break;
        case 12:
            texColor = texture(uTextures[12], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[12], fTexCoords + offsets[i]).a;
            }
            break;
        case 13:
            texColor = texture(uTextures[13], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[13], fTexCoords + offsets[i]).a;
            }
            break;
        case 14:
            texColor = texture(uTextures[14], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[14], fTexCoords + offsets[i]).a;
            }
            break;
        case 15:
            texColor = texture(uTextures[15], fTexCoords);
            for (int i=0; i < 9; i++) {
                sampleTex[i] = texture(uTextures[15], fTexCoords + offsets[i]).a;
            }
            break;
    }

    // If you're not on a Linux machine, you could replace the giant switch with this...
    //if (fTexSlot > 0) {
    //    int texId = int(fTexSlot);
    //    texColor = texture(uTextures[texId], fTexCoords);
    //}

    float midpoint = 0.5;
    float aa = 0.49;

    if (fTexSlot > 0) {
        float c = texColor.r;
        if (c > midpoint)
        {
            color = fColor;
        }
        else if (c > aa)
        {
            c = smoothstep(aa, midpoint, c);
            color = vec4(1, 1, 1, c) * fColor;
        }
        else 
        {
            color = vec4(0, 0, 0, 0);
        }
    } 
    
    entityID = fEntityID;
    
    // TODO Fix this at some point...
    //if (fEntityID == uActiveEntityID) {
    //    float alphaAverage = 0.0;
    //    for (int i=0; i < 9; i++) {
    //        alphaAverage += sampleTex[i] * kernel[i];
    //    }
    //
    //    if (alphaAverage < 0.5 && fTexSlot > 0) {
    //        color = vec4(1, 1, 0, texColor.a);
    //    } else if (fTexSlot == 0) {
    //        color = fColor;
    //    }
    //}
}