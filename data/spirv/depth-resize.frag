#version 460
#extension GL_ARB_separate_shader_objects : enable

#define TWEAK 0.99
#define SSAO_DOWNSCALE 2.0;

//FIXME: this is from UE4, not sure yet if needed.
// needed to prevent AO seam near 16 bit float maximum, this 
//factor pushed the problem far out and it seems to not have
//a visual degradion nearby
const float cf16f_scale = 4096.0f * 32.0f;
float T = 1.0 / SSAO_DOWNSCALE;

//TODO test this too not only max
float checkerboard(in vec2 uv)
{
   vec2 pos = floor(uv);
   return mod(pos.x + mod(pos.y, 2.0), 2.0);
}

float similarity(in float d1, in float d2, float scale)
{
   return clamp(1 - abs(d1 - d2) * scale, 0.0, 1.0).x;
}

void downsample(in sampler2D tex_depth, in sampler2D tex_normal,
                in vec2 in_uv,
                out float max_depth, out vec3 avg_normal)
{
   float depths[] = float[] (
      textureOffset(tex_depth, in_uv, ivec2(0, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(0, 1)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 1)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 0)).x);

   vec3 normals[] = vec3[] (
      textureOffset(tex_normal, in_uv, ivec2(0, 0)).xyz,
      textureOffset(tex_normal, in_uv, ivec2(0, 1)).xyz,
      textureOffset(tex_normal, in_uv, ivec2(1, 1)).xyz,
      textureOffset(tex_normal, in_uv, ivec2(1, 0)).xyz);

   max_depth = max(max(depths[0], depths[1]), max(depths[2], depths[3]));

   avg_normal = vec3(0.0001f, 0.0001f, 0.0001f);
   for (int i = 0; i < 4; i++) {
      avg_normal += normals[i] * similarity(depths[i], max_depth, T);
   }
}

layout(location = 0) in vec2 in_uv;

layout(set = 0, binding = 0) uniform sampler2D tex_depth;
layout(set = 0, binding = 1) uniform sampler2D tex_normal;
layout (location = 0) out vec4 out_color; 

void main()
{
   float depth;
   vec3 normal;
   downsample(tex_depth, tex_normal, in_uv, depth, normal);

   gl_FragDepth = depth;
   out_color = vec4(normal, 1.0);
}
