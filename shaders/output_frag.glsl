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

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

vec3 unpackNormal(vec3 normal)
{
	return (normal * 2.0) - vec3(1.0);
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

	vec3 textureIndex = WSPos;
	textureIndex *= vec3(1.0, 1.0, -1.0); // 3d texture is flipped somehow
	textureIndex += vec3(orthoWidth) - sceneCenter;
	// textureIndex += debugPos;
	textureIndex /= (orthoWidth * 2);

	// vec3 voxelTexAlbedo = texture(voxelAlbedoTex, textureIndex).rgb;
	// vec3 voxelTexNormal = texture(voxelNormalTex, textureIndex).rgb;
	vec3 voxelTexEmissive = textureLod(voxelEmissiveTex, textureIndex, 2.0).rgb;

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metalness);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	// for(int i = 0; i < numLights; ++i)
	for(int i = 0; i < 1; ++i)
	{
		// calculate per-light radiance
		vec3 lightVector = normalize(lightPositions[i] - WSPos);
		vec3 halfVector = normalize(viewVector + lightVector);
		float distance = length(lightPositions[i] - WSPos);
		float attenuation = 1000000.0 / (distance * distance);
		vec3 radiance = vec3(attenuation); //lightColors[i] * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(WSNormal, halfVector, roughness);
		float G   = GeometrySmith(WSNormal, viewVector, lightVector, roughness);
		vec3 F    = fresnelSchlick(max(dot(halfVector, viewVector), 0.0), F0);

		vec3 nominator    = NDF * G * F;
		float denominator = 4 * max(dot(WSNormal, viewVector), 0.0) * max(dot(WSNormal, lightVector), 0.0) + 0.001; // 0.001 to prevent divide by zero.
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
		kD *= 1.0 - metalness;

		// scale light by NdotL
		float NdotL = max(dot(WSNormal, lightVector), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo / PI + specular) * voxelTexEmissive * 10.0; //* radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}

	// ambient lighting term
	// this will probably be where i put my VXGI
	vec3 ambient = vec3(0.03) * albedo;

	vec3 fragShaded = Lo + ambient;

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
