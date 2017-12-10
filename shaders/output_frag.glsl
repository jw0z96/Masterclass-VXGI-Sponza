#version 430
// The texture to be mapped
layout(binding = 0) uniform sampler2D WSPositionTex;
layout(binding = 1) uniform sampler2D WSNormalTex;
layout(binding = 2) uniform sampler2D depthTex;
layout(binding = 3) uniform sampler2D albedoTex;
layout(binding = 4) uniform sampler2D metalRoughTex;

// uniform sampler3D voxelAlbedoTex;
// uniform sampler3D voxelNormalTex;
layout(binding = 5) uniform sampler3D voxelEmissiveTex;

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

// We pass the window size to the shader.
uniform vec2 windowSize;

// lights
uniform int numLights;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;
uniform bool gBufferView;
uniform vec3 debugPos;

const float PI = 3.14159265359;

const vec3 diffuseConeDirections[] =
{
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 0.5f, 0.866025f),
	vec3(0.823639f, 0.5f, 0.267617f),
	vec3(0.509037f, 0.5f, -0.7006629f),
	vec3(-0.50937f, 0.5f, -0.7006629f),
	vec3(-0.823639f, 0.5f, 0.267617f)
};

const float diffuseConeWeights[] =
{
	PI / 4.0f,
	3.0f * PI / 20.0f,
	3.0f * PI / 20.0f,
	3.0f * PI / 20.0f,
	3.0f * PI / 20.0f,
	3.0f * PI / 20.0f,
};

vec3 unpackNormal(vec3 normal)
{
	return (normal * 2.0) - vec3(1.0);
}

vec3 worldToTexCoord(vec3 pos)
{
	float textureSize = (2.0 * orthoWidth);
	vec3 worldMinPoint = sceneCenter - vec3(orthoWidth);
	vec3 result = pos;
	result *= vec3(1.0, 1.0, -1.0); // 3d texture is flipped somehow
	result -= worldMinPoint;
	result /= textureSize;
	return result;
}

// ----------------------------------------------------------------------------

vec3 traceCone(vec3 position, vec3 normal, vec3 direction, float aperture)
{
	// world space grid voxel size
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);

	// move further to avoid self collision
	float dst = voxelSize;
	vec3 startPosition = position + normal * dst;

	// final results
	vec3 result = vec3(0.0);
	float maxTracingDistanceGlobal = 100.0;
	float maxDistance = maxTracingDistanceGlobal * (1.0 / voxelSize);
	maxDistance = voxelSize * 50.0;

	// out of boundaries check
	// float enter = 0.0; float leave = 0.0;

	// if(!IntersectRayWithWorldAABB(position, direction, enter, leave))
	// {
	// 	coneSample.a = 1.0f;
	// }

	while(dst <= maxDistance)
	{
		vec3 conePosition = startPosition + direction * dst;
		// cone expansion and respective mip level based on diameter
		float diameter = 2.0 * aperture * dst;
		float mipLevel = log2(diameter / voxelSize);
		// convert position to texture coord
		vec3 coord = worldToTexCoord(conePosition);
		// get directional sample from anisotropic representation
		result += textureLod(voxelEmissiveTex, coord, mipLevel).rgb;
		// move further into volume
		dst += diameter * 0.5;
	}

	return result;
}

// ----------------------------------------------------------------------------

vec3 calculateIndirectLighting(vec3 albedo, vec3 position, vec3 normal)
{
	vec3 diffuse = vec3(0.0);

	// component greater than zero
	// if(any(greaterThan(albedo, vec3(0.0))))
	// {
		// diffuse cone setup
		const float aperture = 0.57735f;
		vec3 guide = vec3(0.0f, 1.0f, 0.0f);

		if (abs(dot(normal, guide)) == 1.0f)
		{
			guide = vec3(0.0f, 0.0f, 1.0f);
		}

		// Find a tangent and a bitangent
		vec3 right = normalize(guide - dot(normal, guide) * normal);
		vec3 up = cross(right, normal);

		for(int i = 0; i < 6; i++)
		{
			vec3 coneDirection = normal;
			coneDirection += diffuseConeDirections[i].x * right + diffuseConeDirections[i].z * up;
			coneDirection = normalize(coneDirection);
			// cumulative result
			diffuse += traceCone(position, normal, coneDirection, aperture) * diffuseConeWeights[i];
		}

		diffuse.rgb *= albedo;
	// }

	vec3 result = diffuse;

	return result;
}

void main()
{
	// Determine the texture coordinate from the window size
	vec2 texpos = gl_FragCoord.xy / windowSize;

	// Access the world space position texture
	vec3 WSPos = texture(WSPositionTex, texpos).rgb;
	// Access the world space normal texture
	vec3 WSNormal = texture(WSNormalTex, texpos).rgb;
	// Access the screen space depth texture
	float depth = texture(depthTex, texpos).r;
	// Access the albedo texture
	vec3 albedo = texture(albedoTex, texpos).rgb;
	// Access the metallic, roughness, AO texture
	float metalness = texture(metalRoughTex, texpos).r;
	float roughness = texture(metalRoughTex, texpos).g;

	vec3 viewVector = normalize(camPos - WSPos);

	// vec3 voxelTexAlbedo = texture(voxelAlbedoTex, textureIndex).rgb;
	// vec3 voxelTexNormal = texture(voxelNormalTex, textureIndex).rgb;
	vec3 voxelTexEmissive = textureLod(voxelEmissiveTex, worldToTexCoord(WSPos), 0.0).rgb;

	vec3 fragShaded = voxelTexEmissive + calculateIndirectLighting(albedo, WSPos, WSNormal);

	// HDR tonemapping
	fragShaded = fragShaded / (fragShaded + vec3(1.0));
	// gamma correct
	// fragShaded = pow(fragShaded, vec3(1.0/2.2));

	if(gBufferView)
	{
		if (texpos.x >= 0.5)
		{
			if (texpos.y >= 0.5)
			{
				fragColor = vec4(voxelTexEmissive, 1.0);
				// fragColor = vec4(WSPos/1000.0, 1.0);
			}
			else
			{
				fragColor = vec4(fragShaded, 1.0);
				// fragColor = vec4(unpackNormal(voxelTexNormal), 1.0);
			}
		}
		else
		{
			if (texpos.y >= 0.5)
			{
				fragColor = vec4(albedo, 1.0);
				// fragColor = vec4(WSPos / 1000.0, 1.0);
			}
			else
			{
				fragColor = vec4(WSNormal, 1.0);
				// fragColor = vec4(metalness, roughness, 0.0, 1.0);
			}
		}
	}
	else
	{
		fragColor = vec4(fragShaded, 1.0);
	}
}
