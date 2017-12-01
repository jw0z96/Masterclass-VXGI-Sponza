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

// trace the shadow in voxel space
float traceShadow(vec3 position, vec3 lightPos)
{
	float maxTracingDistance = distance(lightPos, position);
	vec3 direction = normalize(lightPos - position);
	// navigation
	float voxelTexSize = 1.0f / float(voxelDim);
	// float voxelTexSize = 0.01;
	// move one voxel further to avoid self collision
	float dst = voxelTexSize * 2.0;
	vec3 samplePos = position + direction * dst;
	// control variables
	float occlusion = 0.0;

	while (occlusion <= 1.0 && dst <= maxTracingDistance)
	{
		// if (samplePos.x < 0.0 || samplePos.y < 0.0
		// 	|| samplePos.z < 0.0 || samplePos.x > 1.0
		// 	|| samplePos.y > 1.0 || samplePos.z > 1.0)
		// {
		// 	break;
		// }

		// float shadow = 0.1;
		float shadow = texelFetch(voxelAlbedoTex, ivec3(samplePos), 0).a;

		occlusion += shadow;

		// move further into volume
		dst += voxelTexSize;
		samplePos = direction * dst + position;
	}

	// float distance = lightDir.length();
	// float distance = sqrt(dot(lightDir, lightDir));

	return 1.0 - occlusion;
	// return occlusion;
}

vec3 calculatePointLight(vec3 lightPos, vec3 position, vec3 normal)
{
	// vec3 lightDir = lightPos - position;
	// float lightDistance = (lightDir * 0.00001).length();
	// lightDir = normalize(lightDir);

	vec3 voxelPos = worldToIndex(position);
	vec3 lightVoxelPos = worldToIndex(lightPos);
	// vec3 lightVoxelDir = lightVoxelPos - voxelPos;

	float visibility = traceShadow(voxelPos, lightVoxelPos);

	vec3 lightIntensity = vec3(1.0);
	return visibility * lightIntensity;
}

vec3 calculateDirectLighting(vec3 position, vec3 normal)
{
	normal = normalize(normal);
	// float voxelSize = (2.0 * orthoWidth) / float(voxelDim);

	// position = position + (normal * voxelSize);

	vec3 directLight = calculatePointLight(lightPositions[0], position, normal);

	return directLight;
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

	// if (any(notEqual(albedo, vec3(0.0, 0.0, 0.0))))
	// {
		vec3 normal = texelFetch(voxelNormalTex, writePos, 0).rgb;
		normal = unpackNormal(normal);
		vec3 worldPos = indexToWorld(writePos);
		radiance = calculateDirectLighting(worldPos, normal);
	// }

	radiance *= albedo;

	imageStore(u_voxelEmissiveTex, writePos, vec4(radiance, 0.0));
}
