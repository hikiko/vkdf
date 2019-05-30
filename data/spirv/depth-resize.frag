#version 460
#extension GL_ARB_separate_shader_objects : enable

float checkerboard(in vec2 uv)
{
   vec2 pos = floor(uv);
   return mod(pos.x + mod(pos.y, 2.0), 2.0);
}

layout(location = 0) in vec2 in_uv;
layout(set = 0, binding = 0) uniform sampler2D tex_depth;
layout (depth_any) out float gl_FragDepth;

void main()
{
   float d1 = textureOffset(tex_depth, in_uv, ivec2(0, 0)).x;
   float d2 = textureOffset(tex_depth, in_uv, ivec2(0, 1)).x;
   float d3 = textureOffset(tex_depth, in_uv, ivec2(1, 1)).x;
   float d4 = textureOffset(tex_depth, in_uv, ivec2(1, 0)).x;

   /*
    * we use a checkerboard to decide if we are going to use
    * the min or the max depth in the downsampling
    */

   gl_FragDepth = mix(max(max(d1, d2), max(d3, d4)),
                      min(min(d1, d2), min(d3, d4)),
                      checkerboard(in_uv));
}
