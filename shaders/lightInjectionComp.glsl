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

ivec3 worldToIndex(vec3 pos)
{
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);
	vec3 worldMinPoint = sceneCenter - vec3(orthoWidth);
	vec3 result = pos;
	result *= vec3(1.0, 1.0, -1.0); // 3d texture is flipped somehow
	result -= worldMinPoint;
	result /= voxelSize;
	return ivec3(result);
}

vec3 unpackNormal(vec3 normal)
{
	return vec3(-1.0) + (normal * 2.0);
}

vec3 traceShadow(vec3 position, vec3 direction, float maxTracingDistance)
{
	// navigation
	float voxelTexSize = 1.0f / voxelDim;
	// move one voxel further to avoid self collision
	float dst = voxelTexSize * 2.0f;
	vec3 samplePos = direction * dst + position;
	// control variables
	float occlusion = 0.0f;
	// accumulated sample
	float traceSample = 0.0f;

	while (occlusion <= 1.0f && dst <= maxTracingDistance)
	{
		if (samplePos.x < 0.0f || samplePos.y < 0.0f
			|| samplePos.z < 0.0f || samplePos.x > 1.0f
			|| samplePos.y > 1.0f || samplePos.z > 1.0f)
		{
			break;
		}

		vec3 traceSample = texelFetch(voxelAlbedoTex, ivec3(samplePos), 0).rgb;

		float shadow = traceSample.r + traceSample.g + traceSample.b;

		occlusion += shadow;

		// move further into volume
		dst += voxelTexSize;
		samplePos = direction * dst + position;
	}

	return 1.0f - (maxTracingDistance * 0.6);
}

vec3 calculatePointLight(vec3 lightPos, vec3 position, vec3 normal, vec3 albedo)
{
	// vec3 lightDir = lightPos - position;
	// float distance = lightDir.length();
	// lightDir = normalize(lightDir);

	vec3 voxelPos = worldToIndex(position);
	vec3 lightVoxelPos = worldToIndex(lightPos);
	vec3 lightVoxelDir = lightVoxelPos - voxelPos;
	float voxelDistance = lightVoxelDir.length();
	lightVoxelDir = normalize(lightVoxelDir);
	float visibility = traceShadow(lightPos, position);

	return albedo * visibility;
}

vec3 calculateDirectLighting(vec3 position, vec3 normal, vec3 albedo)
{
	normal = normalize(normal);
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);

	position = position + (normal * voxelSize);

	albedo = calculatePointLight(lightPositions[0], position, normal, albedo);

	return albedo;
}

void main()
{
	if (gl_GlobalInvocationID.x >= voxelDim ||
		gl_GlobalInvocationID.y >= voxelDim ||
		gl_GlobalInvocationID.z >= voxelDim)
		return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);

	vec3 albedo = texelFetch(voxelAlbedoTex, writePos, 0).rgb;

	// vec3 lightVector = lightPositions[0] - worldPos;
	// float lightVectorLength = dot(lightVector, lightVector) * 0.00001;
	// float NdotL = max(dot(normal, normalize(lightVector)), 0.0);
	// float radiance = NdotL * (1.0 - lightVectorLength);
	// albedo *= radiance;

	vec3 radiance = vec3(0.0);

	if (any(notEqual(albedo, vec3(0.0, 0.0, 0.0))))
	{
		vec3 normal = texelFetch(voxelNormalTex, writePos, 0).rgb;
		normal = unpackNormal(normal);
		vec3 worldPos = indexToWorld(writePos);
		radiance = calculateDirectLighting(worldPos, normal, albedo);
	}

	imageStore(u_voxelEmissiveTex, writePos, vec4(radiance, 0.0));
}
