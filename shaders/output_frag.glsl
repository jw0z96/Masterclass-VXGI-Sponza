#version 430
// The texture to be mapped
uniform sampler2D WSPositionTex;
uniform sampler2D WSNormalTex;
uniform sampler2D depthTex;

// We pass the window size to the shader.
uniform vec2 windowSize;

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

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

	if (texpos.x >= 0.5)
	{
		fragColor = vec4(WSPos, 1.0);
	}
	else
	{
		fragColor = vec4(WSNormal, 1.0);
	}
}

