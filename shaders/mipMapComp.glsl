#version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(binding = 0, rgba8) uniform image3D u_targetMipTex;

uniform sampler3D lowerMip;

void main()
{
	ivec3 writePos = ivec3(gl_GlobalInvocationID);

	vec3 value = vec3(0.0);

	value += texelFetch(lowerMip, writePos + ivec3(0, 0, 0), 0).rgb;
	value += texelFetch(lowerMip, writePos + ivec3(0, 0, 1), 0).rgb;
	value += texelFetch(lowerMip, writePos + ivec3(0, 1, 0), 0).rgb;
	value += texelFetch(lowerMip, writePos + ivec3(0, 1, 1), 0).rgb;
	value += texelFetch(lowerMip, writePos + ivec3(1, 0, 0), 0).rgb;
	value += texelFetch(lowerMip, writePos + ivec3(1, 0, 1), 0).rgb;
	value += texelFetch(lowerMip, writePos + ivec3(1, 1, 0), 0).rgb;
	value += texelFetch(lowerMip, writePos + ivec3(1, 1, 1), 0).rgb;

	value /= 8.0;

	imageStore(u_targetMipTex, writePos, vec4(value, 0.0));
}
