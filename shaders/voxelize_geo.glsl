#version 430

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

flat out int f_axis; //indicate which axis the projection uses
flat out vec4 f_AABB;

uniform int voxelDim;

void main()
{
	// calculate the face normal to find it's dominant axis
	vec3 faceNormal = normalize(cross( v_Pos[1]-v_Pos[0], v_Pos[2]-v_Pos[0]));
	float NdotXAxis = abs(faceNormal.x);
	float NdotYAxis = abs(faceNormal.y);
	float NdotZAxis = abs(faceNormal.z);
	mat4 domProj; // the projection along the dominant axis

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
	// transform vertices to clip space
	newVertPos[0] = domProj * gl_in[0].gl_Position;
	newVertPos[1] = domProj * gl_in[1].gl_Position;
	newVertPos[2] = domProj * gl_in[2].gl_Position;

	//---------------------------------------------------------------------------------

	// //Next we enlarge the triangle to enable conservative rasterization
	// vec4 AABB;
	// vec2 hPixel = vec2( 1.0/voxelDim, 1.0/voxelDim );
	// float pl = 1.4142135637309 / voxelDim ;

	// //calculate AABB of this triangle
	// AABB.xy = newVertPos[0].xy;
	// AABB.zw = newVertPos[0].xy;

	// AABB.xy = min( newVertPos[1].xy, AABB.xy );
	// AABB.zw = max( newVertPos[1].xy, AABB.zw );

	// AABB.xy = min( newVertPos[2].xy, AABB.xy );
	// AABB.zw = max( newVertPos[2].xy, AABB.zw );

	// //Enlarge half-pixel
	// AABB.xy -= hPixel;
	// AABB.zw += hPixel;

	// f_AABB =AABB;

	// //find 3 triangle edge plane
	// vec3 e0 = vec3( newVertPos[1].xy - newVertPos[0].xy, 0 );
	// vec3 e1 = vec3( newVertPos[2].xy - newVertPos[1].xy, 0 );
	// vec3 e2 = vec3( newVertPos[0].xy - newVertPos[2].xy, 0 );
	// vec3 n0 = cross( e0, vec3(0,0,1) );
	// vec3 n1 = cross( e1, vec3(0,0,1) );
	// vec3 n2 = cross( e2, vec3(0,0,1) );

	// //dilate the triangle
	// newVertPos[0].xy = newVertPos[0].xy + pl*( (e2.xy/dot(e2.xy,n0.xy)) + (e0.xy/dot(e0.xy,n2.xy)) );
	// newVertPos[1].xy = newVertPos[1].xy + pl*( (e0.xy/dot(e0.xy,n1.xy)) + (e1.xy/dot(e1.xy,n0.xy)) );
	// newVertPos[2].xy = newVertPos[2].xy + pl*( (e1.xy/dot(e1.xy,n2.xy)) + (e2.xy/dot(e2.xy,n1.xy)) );

	//---------------------------------------------------------------------------------

	gl_Position = newVertPos[0];
	f_Pos = newVertPos[0].xyz;
	f_Normal = v_Normal[0];
	f_TexCoords = v_TexCoords[0];
	EmitVertex();

	gl_Position = newVertPos[1];
	f_Pos = newVertPos[1].xyz;
	f_Normal = v_Normal[1];
	f_TexCoords = v_TexCoords[1];
	EmitVertex();

	gl_Position = newVertPos[2];
	f_Pos = newVertPos[2].xyz;
	f_Normal = v_Normal[2];
	f_TexCoords = v_TexCoords[2];
	EmitVertex();

	EndPrimitive();
}
