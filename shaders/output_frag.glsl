#version 430

// our input textures from our framebuffer object
layout(binding = 0) uniform sampler2D WSPositionTex;
layout(binding = 1) uniform sampler2D WSNormalTex;
layout(binding = 2) uniform sampler2D depthTex;
layout(binding = 3) uniform sampler2D albedoTex;
layout(binding = 4) uniform sampler2D metalRoughTex;

// our input 3d texture from the light injection pass
layout(binding = 5) uniform sampler3D voxelEmissiveTex;

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

// We pass the window size to the shader.
uniform vec2 windowSize;

// light parameters
uniform vec3 lightPosition;
uniform float lightIntensity;
uniform float lightFalloffExponent;

uniform float shadowApertureMultiplier;
uniform float specularApertureMultiplier;
uniform float directLightAmount;
uniform float indirectLightAmount;
uniform float reflectionsAmount;

uniform bool viewDirectLight;
uniform bool viewIndirectLight;
uniform bool viewReflections;

uniform vec3 camPos;

// ----------------------------------------------------------------------------
/// the following variables are copied from this implementation:
/// https://github.com/jose-villegas/VCTRenderer/
/// though are as described in Crassin et al. 2011
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------

// convert normal from [0.0 > 1.0] range to [-1.0 > 1.0] range
vec3 unpackNormal(vec3 normal)
{
	return (normal * 2.0) - vec3(1.0);
}

// convert our position from world space to normalized texture space
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
/// PBR FUNCTIONS
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

// trace a cone and return accumulated radiance
vec3 traceCone(vec3 position, vec3 normal, vec3 direction, float aperture)
{
	// world space grid voxel size
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);

	// move further to avoid self collision
	float dst = voxelSize;
	vec3 startPosition = position + normal * dst;

	// final results
	vec4 result = vec4(0.0);
	float maxDistance = voxelSize * 600.0; //TODO: remove magic number

	while(dst <= maxDistance && result.a < 1.0)
	{
		// set the position to sample
		vec3 conePosition = startPosition + direction * dst;
		// convert position to texture coord
		vec3 coord = worldToTexCoord(conePosition);

		// if the cone escapes, stop tracing
		if (any(lessThan(coord, vec3(0.0))) || any(greaterThan(coord, vec3(1.0))))
			break;

		// cone expansion and respective mip level based on diameter
		float diameter = 2.0 * aperture * dst;
		float mipLevel = log2(diameter / voxelSize);

		// move further into volume
		dst += diameter * 0.5;
		result += textureLod(voxelEmissiveTex, coord, mipLevel);
	}

	return result.rgb;
}

// ----------------------------------------------------------------------------

// trace a cone and return accumulated opacity
float traceShadowCone(vec3 position, vec3 direction, float aperture, float maxDistance)
{
	// world space grid voxel size
	float voxelSize = (2.0 * orthoWidth) / float(voxelDim);

	// move further to avoid self collision
	float dst = voxelSize;
	vec3 startPosition = position + direction * dst;

	// final results
	float visibility = 0.0;

	while(dst <= maxDistance && visibility < 1.0)
	{
		// set the position to sample
		vec3 conePosition = startPosition + direction * dst;
		// convert position to texture coord
		vec3 coord = worldToTexCoord(conePosition);

		// if the cone escapes, stop tracing and return
		if (any(lessThan(coord, vec3(0.0))) || any(greaterThan(coord, vec3(1.0))))
		{
			visibility = 1.0;
			break;
		}

		// cone expansion and respective mip level based on diameter
		float diameter = 2.0 * aperture * dst;
		float mipLevel = log2(diameter / voxelSize);

		// accumulate voxel opacity
		vec4 result = textureLod(voxelEmissiveTex, coord, mipLevel);
		visibility += result.a;

		// move further into volume
		dst += diameter * 0.5;
	}

	return 1.0 - visibility;
}

// ----------------------------------------------------------------------------

// calculate the indirect lighting by tracing 6 weighted diffuse cones
vec3 calculateIndirectLighting(vec3 position, vec3 normal, vec3 albedo, float roughness, float metallic)
{
	// our cone aperture
	const float aperture = 0.57735f;
	vec3 guide = vec3(0.0f, 1.0f, 0.0f);

	if (abs(dot(normal, guide)) == 1.0f)
		guide = vec3(0.0f, 0.0f, 1.0f);

	// Find a tangent and a bitangent
	vec3 right = normalize(guide - dot(normal, guide) * normal);
	vec3 up = cross(right, normal);

	vec3 diffuseCones = vec3(0.0);

	for(int i = 0; i < 6; i++)
	{
		vec3 coneDirection = normal;
		coneDirection += diffuseConeDirections[i].x * right + diffuseConeDirections[i].z * up;
		coneDirection = normalize(coneDirection);
		diffuseCones += traceCone(position, normal, coneDirection, aperture) * diffuseConeWeights[i];
	}

	return diffuseCones * albedo;
}

// ----------------------------------------------------------------------------

// calculate the reflected lighting by tracing a single specular cone
vec3 calculateReflection(vec3 position, vec3 normal, vec3 albedo, float roughness, float metallic)
{
	vec3 viewDirection = normalize(camPos - position);
	vec3 coneDirection = reflect(-viewDirection, normal);
	coneDirection = normalize(coneDirection);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 specularCones = vec3(0.0);

	// for each light
	vec3 lightVector = normalize(lightPosition - position);
	vec3 halfVector = normalize(viewDirection + lightVector);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, halfVector, roughness);
	float G = GeometrySmith(normal, viewDirection, lightVector, roughness);
	vec3 F = fresnelSchlick(max(dot(normal, viewDirection), 0.0), F0);

	// const float specularAperture = 0.57735 / 10.0;
	// TODO: check this
	// specular cone setup, minimum of 1 grad, fewer can severly slow down performance
	// float aperture = clamp(tan(HALF_PI * (1.0f - specular.a)), 0.0174533f, PI);
	float specularAperture = clamp(specularApertureMultiplier * tan(HALF_PI * (1.0 - NDF - G)), 0.0174533f, PI);
	// float specularAperture = clamp(specularApertureMultiplier, 0.0174533f, PI);
	// float specularAperture = NDF;
	specularCones += F * traceCone(position, normal, coneDirection, specularAperture);

	return specularCones;
}

// ----------------------------------------------------------------------------

// calculate the direct light using PBR shading and cone tracing for soft shadows
vec3 calculateDirectLighting(vec3 position, vec3 normal, vec3 albedo, float roughness, float metallic, vec3 reflection)
{
	vec3 viewDirection = normalize(camPos - position);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);

	// for each light calculate per-light radiance
	vec3 lightVector = normalize(lightPosition - position);
	vec3 halfVector = normalize(viewDirection + lightVector);
	float distance = length(lightPosition - position);
	float radiance = lightIntensity / pow((distance / 100.0), lightFalloffExponent);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, halfVector, roughness);
	float G = GeometrySmith(normal, viewDirection, lightVector, roughness);
	vec3 F = fresnelSchlick(max(dot(halfVector, viewDirection), 0.0), F0);

	vec3 nominator = NDF * G * F;
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

	float shadowAperture = shadowApertureMultiplier;
	float visibility = traceShadowCone(position, lightVector, shadowAperture, distance);
	visibility = clamp(visibility, 0.0, 1.0);

	Lo += (kD * albedo / PI + specular) * radiance * visibility; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

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
	// Access the metallic & roughness texture
	float metalness = texture(metalRoughTex, texpos).r;
	float roughness = texture(metalRoughTex, texpos).g;

	vec3 voxelTexEmissive = textureLod(voxelEmissiveTex, worldToTexCoord(WSPos), 0.0).rgb;

	vec3 fragShaded = vec3(0.0);

	if (viewDirectLight)
		fragShaded += directLightAmount * calculateDirectLighting(WSPos, WSNormal, albedo, roughness, metalness, vec3(0.0));

	if (viewIndirectLight)
		fragShaded += indirectLightAmount * calculateIndirectLighting(WSPos, WSNormal, albedo, roughness, metalness);

	if (viewReflections)
		fragShaded += reflectionsAmount * calculateReflection(WSPos, WSNormal, albedo, roughness, metalness);

	// HDR tonemapping
	fragShaded = fragShaded / (fragShaded + vec3(1.0));

	// gamma correct
	fragShaded = pow(fragShaded, vec3(1.0/2.2));

	fragColor = vec4(fragShaded, 0.0);
}
