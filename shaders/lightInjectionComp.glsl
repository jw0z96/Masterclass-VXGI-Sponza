#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(binding = 0, rgba8) uniform image3D u_voxelEmissiveTex;

uniform sampler3D voxelAlbedoTex;
uniform sampler3D voxelNormalTex;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

vec3 indexToWorld(ivec3 pos)
{
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);
	vec3 worldMinPoint = sceneCenter - vec3(orthoWidth);
	vec3 result = vec3(pos);
	result *= voxelSize;
	result += worldMinPoint;
	result *= vec3(1.0, 1.0, -1.0); // 3d texture is flipped somehow
	return result;
}

ivec3 worldToIndex(vec3 position)
{
	float voxelSize = orthoWidth / float(voxelDim);
	vec3 worldMinPoint = sceneCenter - vec3(orthoWidth / 2.0);
	vec3 voxelPos = position - worldMinPoint;
	return ivec3(voxelPos / voxelSize);
}

vec3 unpackNormal(vec3 normal)
{
	return vec3(-1.0) + (normal * 2.0);
}

void main()
{
	if (gl_GlobalInvocationID.x >= voxelDim ||
		gl_GlobalInvocationID.y >= voxelDim ||
		gl_GlobalInvocationID.z >= voxelDim)
		return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);

	vec3 albedo = texelFetch(voxelAlbedoTex, writePos, 0).rgb;
	vec3 normal = texelFetch(voxelNormalTex, writePos, 0).rgb;
	normal = unpackNormal(normal);

	vec3 worldPos = indexToWorld(writePos);

	vec3 lightVector = lightPositions[0] - worldPos;
	float lightVectorLength = dot(lightVector, lightVector) * 0.000001;

	float NdotL = max(dot(normal, normalize(lightVector)), 0.0);

	float radiance = NdotL * (1.0 - lightVectorLength);

	imageStore(u_voxelEmissiveTex, writePos, vec4(vec3(radiance), 0.0));

}
