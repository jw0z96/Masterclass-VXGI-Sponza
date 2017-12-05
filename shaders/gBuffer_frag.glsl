#version 330 core
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

// uniform sampler3D voxelAlbedoTex;
// uniform sampler3D voxelNormalTex;

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

// The output textures that make up our gBuffer
layout (location=0) out vec3 fragWSPosition;
layout (location=1) out vec3 fragWSNormal;
layout (location=2) out vec3 fragAlbedo;
layout (location=3) out vec2 fragMetalRough;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(WorldPos);
	vec3 Q2 = dFdy(WorldPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);

	vec3 N = normalize(Normal);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
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

void main()
{
	if(texture(albedoMap, TexCoords).a==0)
		discard;

	// PBR texture lookups
	vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	// calculate normal-mapped world space normals
	vec3 N = getNormalFromMap();

	vec3 textureIndex = worldToTexCoord(WorldPos);

	// vec3 voxelTexAlbedo = texture(voxelAlbedoTex, textureIndex).rgb;
	// vec3 voxelTexNormal = texture(voxelNormalTex, textureIndex).rgb;

	// world space position out
	fragWSPosition = WorldPos;
	// world space normal out
	fragWSNormal = N;
	// textured fragment albedo out
	fragAlbedo = albedo;
	// packed fragment metallic, roughness, ao textures
	fragMetalRough = vec2(metallic, roughness);
}
