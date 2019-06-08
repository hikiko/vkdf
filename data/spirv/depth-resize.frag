#version 460
#extension GL_ARB_separate_shader_objects : enable

float checkerboard(in vec2 uv)
{
   vec2 pos = floor(uv);
   return mod(pos.x + mod(pos.y, 2.0), 2.0);
}

void downsample(in sampler2D tex_depth, in sampler2D tex_normal,
                in vec2 in_uv,
                out float depth, out vec3 normal)
{
   float d1 = textureOffset(tex_depth, in_uv, ivec2(0, 0)).x;
   float d2 = textureOffset(tex_depth, in_uv, ivec2(0, 1)).x;
   float d3 = textureOffset(tex_depth, in_uv, ivec2(1, 1)).x;
   float d4 = textureOffset(tex_depth, in_uv, ivec2(1, 0)).x;

   /*
    * we use a checkerboard to decide if we are going to use
    * the min or the max depth in the downsampling
    */

   depth = mix(max(max(d1, d2), max(d3, d4)),
               min(min(d1, d2), min(d3, d4)),
               checkerboard(in_uv));

   depth = max(max(d1, d2), max(d3, d4));

   /* then we select the normal of the sample with the selected depth */
   if (depth == d1)
      normal = textureOffset(tex_normal, in_uv, ivec2(0, 0)).xyz;
   else if (depth == d2)
      normal = textureOffset(tex_normal, in_uv, ivec2(0, 1)).xyz;
   else if (depth == d3)
      normal = textureOffset(tex_normal, in_uv, ivec2(1, 0)).xyz;
   else if (depth == d4)
      normal = textureOffset(tex_normal, in_uv, ivec2(1, 1)).xyz;
}

layout(location = 0) in vec2 in_uv;

layout(set = 0, binding = 0) uniform sampler2D tex_depth;
layout(set = 0, binding = 1) uniform sampler2D tex_normal;

//layout (depth_any) out float gl_FragDepth;
layout (location = 0) out vec4 out_color; 

void main()
{
   float depth;
   vec3 normal;
   downsample(tex_depth, tex_normal, in_uv, depth, normal);

   gl_FragDepth = depth;
   out_color = vec4(normal, 1.0);
}
