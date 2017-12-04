#version 430 core

// #extension GL_NV_conservative_raster : enable
#define MAX_COLOR_VALUES 256.0

in vec2 f_TexCoords;
in vec3 f_Pos;
in vec3 f_Normal;

flat in int f_axis; //indicate which axis the projection uses
// flat in vec4 f_AABB;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

layout (location = 0) out vec4 gl_FragColor;
layout (pixel_center_integer) in vec4 gl_FragCoord;

layout(binding = 0, r32ui) uniform volatile coherent uimage3D u_voxelAlbedoTex;
layout(binding = 1, r32ui) uniform volatile coherent uimage3D u_voxelNormalTex;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

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

vec3 packNormal(vec3 normal)
{
	return (normal * 0.5)  + vec3(0.5);
}

void main()
{
	// if( f_Pos.x < f_AABB.x || f_Pos.y < f_AABB.y || f_Pos.x > f_AABB.z || f_Pos.y > f_AABB.w )
	// 	discard ;

	if(texture(albedoMap, f_TexCoords).a==0)
		discard;

	// PBR texture lookups
	vec3 albedo = pow(texture(albedoMap, f_TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, f_TexCoords).r;
	float roughness = texture(roughnessMap, f_TexCoords).r;

	// calculate normal-mapped world space normals
	vec3 N = f_Normal;

	vec3 temp = vec3(gl_FragCoord.x, gl_FragCoord.y, voxelDim * gl_FragCoord.z);
	// temp -= sceneCenter;
	// temp *= orthoWidth;

	ivec3 texcoord;
	if( f_axis == 1 )
	{
		texcoord.x = int(voxelDim - temp.z);
		texcoord.z = int(temp.x);
		texcoord.y = int(temp.y);
	}
	else if( f_axis == 2 )
	{
		texcoord.z = int(temp.y);
		texcoord.y = int(voxelDim-temp.z);
		texcoord.x = int(temp.x);
	}
	else
		texcoord = ivec3(temp);

	imageAtomicRGBA8Avg(u_voxelAlbedoTex, texcoord, vec4(albedo, 1.0));
	imageAtomicRGBA8Avg(u_voxelNormalTex, texcoord, vec4(packNormal(N), 1.0));
}
