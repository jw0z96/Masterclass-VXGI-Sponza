#version 430

// ----------------------------------------------------------------------------
/// the following shader is inspired one used in this implementation,
/// though uses the methods described in Ch. 22 of OpenGLInsights
/// https://github.com/otaku690/SparseVoxelOctree
// ----------------------------------------------------------------------------

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_Pos[];
in vec3 v_Normal[];
in vec2 v_TexCoords[];

out vec3 f_Normal;
out vec3 f_Pos;
out vec2 f_TexCoords;

uniform mat4 mvpX;
uniform mat4 mvpY;
uniform mat4 mvpZ;

// dominant projection axis to send to frag shader
flat out int f_axis;

uniform int voxelDim;

void main()
{
	// calculate the face normal to find it's dominant axis
	vec3 faceNormal = normalize(cross(v_Pos[1] - v_Pos[0], v_Pos[2] - v_Pos[0]));
	float NdotXAxis = abs(faceNormal.x);
	float NdotYAxis = abs(faceNormal.y);
	float NdotZAxis = abs(faceNormal.z);

	// matrix for the dominant axis projection
	mat4 domProj;
	if (NdotXAxis > NdotYAxis && NdotXAxis > NdotZAxis)
	{
		domProj = mvpX;
		f_axis = 1;
	}
	else if (NdotYAxis > NdotXAxis && NdotYAxis > NdotZAxis)
	{
		domProj = mvpY;
		f_axis = 2;
	}
	else
	{
		domProj = mvpZ;
		f_axis = 3;
	}

	// verts of our new triangle
	vec4 newVertPos[3];
	for (int i = 0; i < 3; ++i)
	{
		newVertPos[i] = domProj * gl_in[i].gl_Position;
		gl_Position = newVertPos[i];
		f_Pos = newVertPos[i].xyz;
		f_Normal = v_Normal[i];
		f_TexCoords = v_TexCoords[i];
		EmitVertex();
	}

	EndPrimitive();
}
