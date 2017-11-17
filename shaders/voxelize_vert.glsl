#version 410 core

/// @brief the vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief the normal passed in
layout (location = 2) in vec3 inNormal;
/// @brief the in uv
layout (location = 1) in vec2 inUV;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 orthoMVP;

void main()
{
	// pass texture coordinate to geometry shader
	TexCoords = inUV;
	// pass worldspace position to geometry shader
	WorldPos = inVert;
	// pass worldspace normal to geometry shader
	Normal = inNormal;
	// transform input vertex
	gl_Position = orthoMVP * vec4(inVert, 1.0);
}
