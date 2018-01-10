#version 430

const float EPSILON = 1e-30;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

// our 3d texture to be written to
layout(binding = 0, rgba8) uniform image3D u_voxelEmissiveTex;

// our 3d textures obtained by voxelize pass
uniform sampler3D voxelAlbedoTex;
uniform sampler3D voxelNormalTex;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

// light parameters
uniform vec3 lightPosition;
uniform float lightIntensity;
uniform float lightFalloffExponent;

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
	// navigate through the texture using the size of a single voxel
	float voxelTexSize = 1.0;
	// move one voxel further to avoid self collision
	float dst = voxelTexSize * 2.0;

	// control variables
	float occlusion = 0.0;
	vec3 samplePos;

	while (dst <= maxTracingDistance)
	{
		// set the position to sample
		samplePos = position + (direction * dst);

		// if the sample position escapes the normalized coordinates, stop tracing
		if (any(lessThan(samplePos, vec3(0.0))) ||
			any(greaterThan(samplePos, vec3(voxelDim))))
			break;

		// if the voxel at the sample position has opacity, return
		if (texelFetch(voxelAlbedoTex, ivec3(samplePos), 0).a > EPSILON)
			return 0.0;

		// move further into volume
		dst += voxelTexSize;
	}

	return 1.0 - occlusion;
}

vec3 calculatePointLight(vec3 lightPos, vec3 position, vec3 normal)
{
	// vector between light and position in world space
	vec3 lightDir = lightPos - position;
	lightDir = normalize(lightDir);

	// calculate directional normal attenuation
	vec3 weight = normal * normal;

	float rDotL = dot(vec3(1.0, 0.0, 0.0), lightDir);
	float uDotL = dot(vec3(0.0, 1.0, 0.0), lightDir);
	float fDotL = dot(vec3(0.0, 0.0, 1.0), lightDir);

	rDotL = normal.x > 0.0 ? max(rDotL, 0.0) : max(-rDotL, 0.0);
	uDotL = normal.y > 0.0 ? max(uDotL, 0.0) : max(-uDotL, 0.0);
	fDotL = normal.z > 0.0 ? max(fDotL, 0.0) : max(-fDotL, 0.0);

	// voxel shading average from all front sides
	float NdotL = rDotL * weight.x + uDotL * weight.y + fDotL * weight.z;

	// dot product of surface normal and light vector, gives n.l term
	// float NdotL = max(dot(normal, lightDir), 0.0);

	// get the surface position and light position voxels in texture
	// coordinates [0,0,0 - 256,256,256]
	vec3 voxelPos = worldToIndex(position);
	vec3 lightVoxelPos = worldToIndex(lightPos);
	// vector between light and position in texture coordinate space
	vec3 lightVoxelDir = lightVoxelPos - voxelPos;
	lightVoxelDir = normalize(lightVoxelDir);
	// distance between light and position in texture coordinate space
	float lightVoxelDistance = distance(lightVoxelPos, voxelPos);
	// calculate whether the position voxel is in shadow through raytracing
	float visibility = traceShadow(voxelPos, lightVoxelDir, lightVoxelDistance);
	// calculate light falloff
	float distance = distance(lightPos, position);
	float radiance = lightIntensity / pow((distance / 100.0), lightFalloffExponent);

	vec3 lightColor = vec3(1.0);
	return lightColor * visibility * NdotL * radiance;
}

vec3 calculateDirectLighting(vec3 position, vec3 normal)
{
	normal = normalize(normal);
	// calculate the size of one voxel in world space
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);
	// move to the next voxel above the surface in world space,
	// to avoid self shadowing artefact
	position = position + (normal * ceil(voxelSize));

	vec3 directLight = vec3(0.0);

	// for each light acculmulate direct lighting
	directLight += calculatePointLight(lightPosition, position, normal);

	return directLight;
}

void main()
{
	if (gl_GlobalInvocationID.x >= voxelDim ||
		gl_GlobalInvocationID.y >= voxelDim ||
		gl_GlobalInvocationID.z >= voxelDim)
		return;

	ivec3 writePos = ivec3(gl_GlobalInvocationID);

	vec4 albedoTex = texelFetch(voxelAlbedoTex, writePos, 0);

	vec3 radiance = vec3(0.0);
	float opacity = 0.0;

	// if our voxel isnt empty, calculate the direct lighting it recieves
	if (any(notEqual(albedoTex, vec4(0.0))))
	{
		opacity = 1.0;
		vec3 normal = texelFetch(voxelNormalTex, writePos, 0).rgb;
		vec3 worldPos = indexToWorld(writePos);
		radiance = calculateDirectLighting(worldPos, unpackNormal(normal));
		radiance *= albedoTex.rgb;
	}

	// store the emmisive value in the 3d texture
	imageStore(u_voxelEmissiveTex, writePos, vec4(radiance, opacity));
}
