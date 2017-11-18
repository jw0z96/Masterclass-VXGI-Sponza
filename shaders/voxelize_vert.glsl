#version 410 core

/// @brief the vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief the normal passed in
layout (location = 2) in vec3 inNormal;
/// @brief the in uv
layout (location = 1) in vec2 inUV;

out vec2 v_TexCoords;
out vec3 v_Pos;
out vec3 v_Normal;

void main()
{
	// pass texture coordinate to geometry shader
	v_TexCoords = inUV;
	// pass worldspace position to geometry shader
	v_Pos = inVert;
	// pass worldspace normal to geometry shader
	v_Normal = inNormal;
	// transform input vertex
	gl_Position = vec4(inVert, 1.0);
}
