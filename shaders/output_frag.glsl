#version 430
// The texture to be mapped
uniform sampler2D WSPositionTex;
uniform sampler2D WSNormalTex;
uniform sampler2D depthTex;
uniform sampler2D albedoTex;
uniform sampler2D metalRoughTex;

// We pass the window size to the shader.
uniform vec2 windowSize;

// // lights
uniform vec3 lightPositions[4];
// uniform vec3 lightColors[4];

uniform vec3 camPos;

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

void main()
{
	// Determine the texture coordinate from the window size
	vec2 texpos = gl_FragCoord.xy / windowSize;

	// Access the world space position texture
	vec3 WSPos = texture(WSPositionTex, 2.0 * texpos).rgb;
	// Access the world space normal texture
	vec3 WSNormal = texture(WSNormalTex, 2.0 * texpos).rgb;
	// Access the screen space depth texture
	float depth = texture(depthTex, 2.0 * texpos).r;
	// Access the albedo texture
	vec3 albedo = texture(albedoTex, 2.0 * texpos).rgb;
	// Access the metallic, roughness, AO texture
	float metalness = texture(metalRoughTex, 2.0 * texpos).r;
	float roughness = texture(metalRoughTex, 2.0 * texpos).g;

	if (texpos.x >= 0.5)
	{
		if (texpos.y >= 0.5)
		{
			fragColor = vec4(WSPos / 1000.0, 1.0);
		}
		else
		{
			fragColor = vec4(albedo, 1.0);
		}
	}
	else
	{
		if (texpos.y >= 0.5)
		{
			fragColor = vec4(WSNormal, 1.0);
		}
		else
		{
			fragColor = vec4(metalness, roughness, 0.0, 1.0);
		}
	}
}

