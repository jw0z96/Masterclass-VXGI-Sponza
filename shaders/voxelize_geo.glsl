// Sparse Voxel Octree and Voxel Cone Tracing
//
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin

//Geometry shader for scene voxelization
//Using the method described in Ch. 22, OpenGL Insights

# version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_position[];
in vec3 v_normal[];
in vec2 v_texcoord[];

out vec3 Normal;
out vec3 WorldPos;
out vec2 TexCoords;

uniform mat4 mvpX;
uniform mat4 mvpY;
uniform mat4 mvpZ;

void main()
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}
