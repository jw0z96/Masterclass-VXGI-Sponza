#version 430 core

// #extension GL_NV_conservative_raster : enable

in vec2 f_TexCoords;
in vec3 f_Pos;
in vec3 f_Normal;
flat in int f_axis; //indicate which axis the projection uses

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

// // The output textures that make up our gBuffer
// layout (location=0) out vec3 fragWSPosition;
// layout (location=1) out vec3 fragWSNormal;
// layout (location=2) out vec3 fragAlbedo;
// layout (location=3) out vec2 fragMetalRough;

uniform layout(binding = 0, rgba8) writeonly image3D u_voxelTex;
// uniform layout(binding = 0, r32ui) coherent uimage3D u_voxelTex;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

vec4 convRGBA8ToVec4(uint val)
{
	return vec4(float((val &0x000000FF)), float((val &0x0000FF00)>>8U), float((val &0x00FF0000)>>16U), float((val &0xFF000000)>>24U));
}

uint convVec4ToRGBA8(vec4 val)
{
	return (uint(val.w) &0x000000FF)<<24U | (uint(val.z) &0x000000FF)<<16U | (uint(val.y) &0x000000FF)<<8U | (uint(val.x) &0x000000FF);
}

void imageAtomicRGBA8Avg(layout(r32ui) coherent volatile uimage3D imgUI, ivec3 coords, vec4 val)
{
	val.rgb *= 255.0f; // Optimise following calculations
	uint newVal = convVec4ToRGBA8(val);
	uint prevStoredVal = 0;
	uint curStoredVal;
	// Loop as long as destination value gets changed by other threads
	while ((curStoredVal = imageAtomicCompSwap(imgUI, coords, prevStoredVal, newVal)) != prevStoredVal)
	{
		prevStoredVal = curStoredVal;
		vec4 rval = convRGBA8ToVec4(curStoredVal);
		rval.xyz = (rval.xyz * rval .w); // Denormalize
		vec4 curValF = rval + val; // Add new value
		curValF. xyz /=(curValF.w); // Renormalize
		newVal = convVec4ToRGBA8(curValF);
	}
}

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, f_TexCoords).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(f_Pos);
	vec3 Q2 = dFdy(f_Pos);
	vec2 st1 = dFdx(f_TexCoords);
	vec2 st2 = dFdy(f_TexCoords);

	vec3 N = normalize(f_Normal);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

void main()
{
	if(texture(albedoMap, f_TexCoords).a==0)
		discard;

	// PBR texture lookups
	vec3 albedo = pow(texture(albedoMap, f_TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, f_TexCoords).r;
	float roughness = texture(roughnessMap, f_TexCoords).r;

	// calculate normal-mapped world space normals
	vec3 N = getNormalFromMap();

	vec3 temp = vec3(gl_FragCoord.x, gl_FragCoord.y, voxelDim * gl_FragCoord.z);
	// temp -= sceneCenter;
	// temp *= orthoWidth;

	ivec3 texcoord;
	if( f_axis == 1 )
	{
		texcoord.x = int(voxelDim - temp.z);
		texcoord.z = int(temp.x);
		texcoord.y = int(temp.y);
	}
	else if( f_axis == 2 )
	{
		texcoord.z = int(temp.y);
		texcoord.y = int(voxelDim-temp.z);
		texcoord.x = int(temp.x);
	}
	else
		texcoord = ivec3(temp);

	imageStore(u_voxelTex, texcoord, vec4(albedo, 1.0));
	// imageAtomicExchange(u_voxelTex, texcoord, convVec4ToRGBA8(vec4(albedo, 1.0)));
	// imageAtomicRGBA8Avg(u_voxelTex, texcoord, vec4(albedo, 0.0));
	// imageStore(u_voxelTex, ivec3(0, 0, 0), vec4(vec3(1.0, 0.0, 0.0), 1.0));
	// imageStore(u_voxelTex, ivec3(-1000, -1000, -1000), vec4(vec3(0.0, 1.0, 0.0), 1.0));

	// // world space position out
	// fragWSPosition = f_Pos;
	// // world space normal out
	// fragWSNormal = N;
	// // textured fragment albedo out
	// fragAlbedo = albedo;
	// // packed fragment metallic, roughness, ao textures
	// fragMetalRough = vec2(metallic, roughness);
}
