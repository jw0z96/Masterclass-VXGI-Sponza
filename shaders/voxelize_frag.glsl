#version 430 core

// ----------------------------------------------------------------------------
/// the following shader is inspired one used in this implementation,
/// though uses the methods described in Ch. 22 of OpenGLInsights
/// https://github.com/otaku690/SparseVoxelOctree
// ----------------------------------------------------------------------------

#define MAX_COLOR_VALUES 256.0

in vec2 f_TexCoords;
in vec3 f_Pos;
in vec3 f_Normal;

// the dominant projection axis
flat in int f_axis;

// material parameters
uniform sampler2D albedoMap;

layout (location = 0) out vec4 gl_FragColor;
layout (pixel_center_integer) in vec4 gl_FragCoord;

layout(binding = 0, r32ui) uniform volatile coherent uimage3D u_voxelAlbedoTex;
layout(binding = 1, r32ui) uniform volatile coherent uimage3D u_voxelNormalTex;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

// ----------------------------------------------------------------------------
/// the following function is described in Ch. 22 of OpenGLInsights:
/// https://www.seas.upenn.edu/~pcozzi/OpenGLInsights/OpenGLInsights-SparseVoxelization.pdf
// ----------------------------------------------------------------------------
void imageAtomicRGBA8Avg(layout(r32ui) coherent volatile uimage3D imgUI, ivec3 coords, vec4 val)
{
	uint newVal = packUnorm4x8(val);
	uint prevStoredVal = 0;
	uint curStoredVal;
	// Loop as long as destination value gets changed by other threads
	while ((curStoredVal = imageAtomicCompSwap(imgUI, coords, prevStoredVal, newVal)) != prevStoredVal)
	{
		prevStoredVal = curStoredVal;
		vec4 rval = unpackUnorm4x8(curStoredVal);
		rval.w *= MAX_COLOR_VALUES;
		rval.xyz = (rval.xyz * rval.w); // Denormalize
		vec4 curValF = rval + val; // Add new value
		curValF.xyz /= (curValF.w); // Renormalize
		curValF.w /= MAX_COLOR_VALUES;
		newVal = packUnorm4x8(curValF);
	}
}
// ----------------------------------------------------------------------------

// convert normal from [-1.0 > 1.0] range to [0.0 > 1.0] range
vec3 packNormal(vec3 normal)
{
	return (normal * 0.5)  + vec3(0.5);
}

void main()
{
	if(texture(albedoMap, f_TexCoords).a==0)
		discard;

	// PBR texture lookups
	vec3 albedo = pow(texture(albedoMap, f_TexCoords).rgb, vec3(2.2));

	vec3 temp = vec3(gl_FragCoord.x, gl_FragCoord.y, voxelDim * gl_FragCoord.z);

	// calculate the 3d texture index given the dominant projection axis
	ivec3 texcoord;
	if(f_axis == 1)
	{
		texcoord.x = int(voxelDim - temp.z);
		texcoord.z = int(temp.x);
		texcoord.y = int(temp.y);
	}
	else if(f_axis == 2)
	{
		texcoord.z = int(temp.y);
		texcoord.y = int(voxelDim - temp.z);
		texcoord.x = int(temp.x);
	}
	else
		texcoord = ivec3(temp);

	// store the fragment in our 3d texture using a moving average
	imageAtomicRGBA8Avg(u_voxelAlbedoTex, texcoord, vec4(albedo, 1.0));
	imageAtomicRGBA8Avg(u_voxelNormalTex, texcoord, vec4(packNormal(f_Normal), 1.0));
}
