#version 430 core
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

uniform int voxelDim;
uniform float orthoWidth;
uniform vec3 sceneCenter;

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
