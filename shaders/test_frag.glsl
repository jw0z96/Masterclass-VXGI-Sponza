#version 430
// The texture to be mapped
uniform sampler2D inTex;
// We pass the window size to the shader.
uniform vec2 windowSize;

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

void main()
{
	// Determine the texture coordinate from the window size
	vec2 texpos = gl_FragCoord.xy / windowSize;

	// Access the testing texture
	vec3 test = texture(inTex, texpos).rgb;

	fragColor = vec4(test, 1.0);

}
