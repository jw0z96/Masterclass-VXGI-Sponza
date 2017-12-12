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
const float HALF_PI = 1.57079f;

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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
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
	// float maxTracingDistanceGlobal = 100.0;
	// float maxDistance = maxTracingDistanceGlobal * (1.0 / voxelSize);
	float maxDistance = voxelSize * 600.0;

	while(dst <= maxDistance)
	{
		vec3 conePosition = startPosition + direction * dst;
		// convert position to texture coord
		vec3 coord = worldToTexCoord(conePosition);

		// cone expansion and respective mip level based on diameter
		float diameter = 2.0 * aperture * dst;
		float mipLevel = log2(diameter / voxelSize);

		result += textureLod(voxelEmissiveTex, coord, mipLevel).rgb;

		// move further into volume
		dst += diameter * 0.5;
	}

	return result.rgb;
}

// ----------------------------------------------------------------------------

vec3 calculateIndirectLighting(vec3 position, vec3 normal, vec3 albedo, float roughness, float metallic)
{
	// DIFFUSE CONES
	vec3 diffuseCones = vec3(0.0);

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
		diffuseCones += traceCone(position, normal, coneDirection, aperture) * diffuseConeWeights[i];
	}

	return diffuseCones * albedo;
}

// ----------------------------------------------------------------------------

vec3 calculateReflection(vec3 position, vec3 normal, vec3 albedo, float roughness, float metallic)
{
	// SPECULAR CONES
	vec3 specularCones = vec3(0.0);
	vec3 viewDirection = normalize(camPos - position);
	vec3 coneDirection = reflect(-viewDirection, normal);
	coneDirection = normalize(coneDirection);
	// specular cone setup, minimum of 1 grad, fewer can severly slow down performance
	// float aperture = clamp(tan(HALF_PI * (1.0f - specular.a)), 0.0174533f, PI);
	// const float specularAperture = 0.57735 / 10.0;


	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	for(int i = 0; i < 1; ++i)
	{
		vec3 lightVector = normalize(lightPositions[i] - position);
		vec3 halfVector = normalize(viewDirection + lightVector);

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(normal, halfVector, roughness);
		float G   = GeometrySmith(normal, viewDirection, lightVector, roughness);
		vec3 F = fresnelSchlick(max(dot(normal, viewDirection), 0.0), F0);

		// const float specularAperture = 0.57735 / 10.0;
		float specularAperture = clamp(tan(HALF_PI * (1.0 - NDF - G)), 0.0174533f, PI);
		// float specularAperture = NDF;
		specularCones += traceCone(position, normal, coneDirection, specularAperture);
		specularCones *= F;
	}

	return specularCones;
}

// ----------------------------------------------------------------------------

vec3 calculateDirectLighting(vec3 position, vec3 normal, vec3 albedo, float roughness, float metallic, vec3 reflection)
{
	vec3 viewDirection = normalize(camPos - position);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	// for(int i = 0; i < numLights; ++i)
	for(int i = 0; i < 1; ++i)
	{
		// calculate per-light radiance
		vec3 lightVector = normalize(lightPositions[i] - position);
		vec3 halfVector = normalize(viewDirection + lightVector);
		float distance = length(lightPositions[i] - position);
		float attenuation = 1000000.0 / (distance * distance);
		vec3 radiance = vec3(attenuation); //lightColors[i] * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(normal, halfVector, roughness);
		float G   = GeometrySmith(normal, viewDirection, lightVector, roughness);
		vec3 F    = fresnelSchlick(max(dot(halfVector, viewDirection), 0.0), F0);

		vec3 nominator    = NDF * G * F;
		float denominator = 4 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightVector), 0.0) + 0.001; // 0.001 to prevent divide by zero.
		vec3 specular = nominator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(normal, lightVector), 0.0);

		vec3 voxelTexEmissive = textureLod(voxelEmissiveTex, worldToTexCoord(position), 0.0).rgb;
		// float shadow = (voxelTexEmissive.r + voxelTexEmissive.g + voxelTexEmissive.b) / 2.0;
		// if (any(greaterThan(voxelTexEmissive, vec3(0.0))))
			// shadow = 1.0;
		// add to outgoing radiance Lo
		// Lo += (kD * albedo / PI + specular) * NdotL * 10.0 * shadow;
		// Lo = specular;
		// Lo += albedo * kD * 10.0 * (voxelTexEmissive.r + voxelTexEmissive.g + voxelTexEmissive.b); // * radiance  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		Lo += (kD * albedo / PI + specular) * 10.0 * (voxelTexEmissive.r + voxelTexEmissive.g + voxelTexEmissive.b); // * radiance  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		// Lo += (kD * albedo / PI + specular) * NdotL * radiance; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		// Lo += (kD * albedo / PI + specular) * 10.0 * (voxelTexEmissive.r + voxelTexEmissive.g + voxelTexEmissive.b); //* radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		// Lo = albedo * NdotL * (voxelTexEmissive.r + voxelTexEmissive.g + voxelTexEmissive.b);
	}

	return Lo;
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

	// vec3 viewVector = normalize(camPos - WSPos);

	// vec3 voxelTexAlbedo = texture(voxelAlbedoTex, textureIndex).rgb;
	// vec3 voxelTexNormal = texture(voxelNormalTex, textureIndex).rgb;
	vec3 voxelTexEmissive = textureLod(voxelEmissiveTex, worldToTexCoord(WSPos), 0.0).rgb;

	vec3 indirectLighting = calculateIndirectLighting(WSPos, WSNormal, albedo, roughness, metalness);
	vec3 reflection = calculateReflection(WSPos, WSNormal, albedo, roughness, metalness);
	vec3 Lo = calculateDirectLighting(WSPos, WSNormal, albedo, roughness, metalness, vec3(0.0));
	// vec3 Lo = calculateDirectLighting(WSPos, WSNormal, albedo, roughness, metalness, reflection);
	vec3 fragShaded = Lo + indirectLighting + reflection;
	// vec3 fragShaded = Lo;
	// HDR tonemapping
	fragShaded = fragShaded / (fragShaded + vec3(1.0));
	// gamma correct
	fragShaded = pow(fragShaded, vec3(1.0/2.2));

	if(gBufferView)
	{
		if (texpos.x >= 0.5)
		{
			if (texpos.y >= 0.5)
			{
				fragColor = vec4(fragShaded, 1.0);
				// fragColor = vec4(WSPos/1000.0, 1.0);
			}
			else
			{
				fragColor = vec4(Lo, 1.0);
				// fragColor = vec4(unpackNormal(voxelTexNormal), 1.0);
			}
		}
		else
		{
			if (texpos.y >= 0.5)
			{
				fragColor = vec4(indirectLighting, 1.0);
				// fragColor = vec4(WSPos / 1000.0, 1.0);
			}
			else
			{
				fragColor = vec4(reflection, 1.0);
				// fragColor = vec4(metalness, roughness, 0.0, 1.0);
			}
		}
	}
	else
	{
		fragColor = vec4(fragShaded, 1.0);
	}
}
