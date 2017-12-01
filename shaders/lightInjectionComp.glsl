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

uniform int numLights;

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
	return (normal * 2.0) - vec3(1.0);
}

// trace the shadow in voxel space
float traceShadow(vec3 position, vec3 direction, float maxTracingDistance)
{
	// navigation
	float voxelTexSize = 1.0 / float(voxelDim);
	// float voxelTexSize = 10.0f;
	// float voxelTexSize = (2.0 * orthoWidth) / float(voxelDim);
	// move one voxel further to avoid self collision
	float dst = voxelTexSize * 2.0;
	// float dst = 10.0;
	// vec3 samplePos = position + direction * dst * 2.0;
	// control variables
	float occlusion = 0.0;

	while (dst <= maxTracingDistance)
	{
		// if (samplePos.x < 0.0 || samplePos.y < 0.0
		// 	|| samplePos.z < 0.0 || samplePos.x > 1.0
		// 	|| samplePos.y > 1.0 || samplePos.z > 1.0)
		// {
		// 	break;
		// }

		// move further into volume
		dst += voxelTexSize;
		vec3 samplePos = position + (direction * dst);

		if (texelFetch(voxelAlbedoTex, ivec3(samplePos), 0).a > 0.0)
		{
			occlusion = 1.0;
			break;
		}
	}

	return 1.0 - occlusion;
}

vec3 calculatePointLight(vec3 lightIntensity, vec3 lightPos, vec3 position, vec3 normal)
{
	vec3 lightDir = lightPos - position;
	lightDir = normalize(lightDir);

	float NdotL = max(dot(normal, lightDir), 0.0);

	vec3 voxelPos = worldToIndex(position);
	vec3 lightVoxelPos = worldToIndex(lightPos);
	vec3 lightVoxelDir = lightVoxelPos - voxelPos;
	float lightVoxelDistance = distance(lightVoxelPos, voxelPos);
	lightVoxelDir = normalize(lightVoxelDir);

	float visibility = traceShadow(voxelPos, lightVoxelDir, lightVoxelDistance);

	float lightDistance = distance(lightPos, position);
	lightDistance /= 1000.0;
	float falloff = 1.0 - (lightDistance * lightDistance);
	falloff = clamp(falloff, 0.0, 1.0);

	return lightIntensity * visibility * NdotL * falloff;
}

vec3 calculateDirectLighting(vec3 position, vec3 normal)
{
	normal = normalize(normal);
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);

	position = position + (normal * voxelSize);

	vec3 directLight = vec3(0.0);

	for (int i = 0; i < numLights; ++i)
	{
		directLight += calculatePointLight(lightColors[i], lightPositions[i], position, normal);
	}

	return directLight / numLights;
}

void main()
{
	if (gl_GlobalInvocationID.x >= voxelDim ||
		gl_GlobalInvocationID.y >= voxelDim ||
		gl_GlobalInvocationID.z >= voxelDim)
		return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);

	vec3 albedo = texelFetch(voxelAlbedoTex, writePos, 0).rgb;

	vec3 radiance = vec3(0.0);

	if (any(notEqual(albedo, vec3(0.0, 0.0, 0.0))))
	{
		vec3 normal = texelFetch(voxelNormalTex, writePos, 0).rgb;
		normal = unpackNormal(normal);
		vec3 worldPos = indexToWorld(writePos);
		radiance = calculateDirectLighting(worldPos, normal);
	}

	radiance *= albedo;

	imageStore(u_voxelEmissiveTex, writePos, vec4(radiance, 0.0));
}
