#type vertex
#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float texID;
layout (location = 4) in float aEntityID;

uniform mat4 uView;
uniform mat4 uProjection;

out float fEntityID;
out vec2 fTexCoords;
out float fTexSlot;

void main()
{
	fEntityID = aEntityID;
	fTexCoords = aTexCoords;
    fTexSlot = texID;

	gl_Position = uProjection * uView * vec4(aPos, 1.0);
}

#type fragment
#version 330

out vec4 FragColor;

void main() 
{
	FragColor = vec4(1.0, 0.28, 0.26, 1.0);
}