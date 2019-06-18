#version 460
#extension GL_ARB_separate_shader_objects : enable

float checkerboard(in vec2 uv)
{
   vec2 pos = floor(uv);
   return mod(pos.x + mod(pos.y, 2.0), 2.0);
}

float most_representative(sampler2D tex_depth, vec2 in_uv)
{
   float d[] = float[] (
      textureOffset(tex_depth, in_uv, ivec2(0, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(0, 1)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 1)).x);

   float centr = (d[0] + d[1] + d[2] + d[3]) / 4.0;
   float dist[] = float[] (
         abs(centr - d[0]),
         abs(centr - d[1]),
         abs(centr - d[2]),
         abs(centr - d[3]));
   
   float max_dist = max(max(dist[0], dist[1]), max(dist[2], dist[3]));
   float d3[3];

   int j = 0;
   for (int i = 0; i < 4; i++) {
      if (dist[i] <= max_dist && j < 3) {
         d3[j] = d[i];
         j++;
      }
   }

   centr = (d3[0] + d3[1] + d3[2]) / 3.0;
   dist[0] = abs(d3[0] - centr);
   dist[1] = abs(d3[1] - centr);
   dist[2] = abs(d3[2] - centr);

   float d2[2]; j = 0;
   max_dist = max(max(dist[0], dist[1]), dist[2]);
   for (int i = 0; i < 3; i++) {
      if (dist[i] <= max_dist && j < 2) {
         d2[j] = d3[i];
         j++;
      }
   }

   centr = (d2[0] + d2[1]) / 2.0;
   dist[0] = abs(d2[0] - centr);
   dist[1] = abs(d2[1] - centr);
   if (dist[0] < dist[1])
      return d2[0];

   return d2[1];
}

float least_representative(sampler2D tex_depth,
                           vec2 in_uv)
{
   float d[] = float[] (
      textureOffset(tex_depth, in_uv, ivec2(0, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(0, 1)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 1)).x);

   float max_dist = 0.0;
   float depth_centroid = (d[0] + d[1] + d[2] + d[3]) / 4.0;
   float least_rep;

   for (int i = 0; i < 4; i++) {
      float dist = abs(d[i] - depth_centroid);
      if (dist > max_dist) {
         max_dist = dist;
         least_rep = d[i];
      }
   }
   return least_rep;
}

layout(set = 0, binding = 0) uniform sampler2D tex_depth;
layout(set = 0, binding = 1) uniform sampler2D tex_normal;
layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 out_color;

void main()
{
   float d[] = float[] (
      textureOffset(tex_depth, in_uv, ivec2(0, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(0, 1)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 1)).x);

   vec3 n[] = vec3[] (
      textureOffset(tex_normal, in_uv, ivec2(0, 0)).xyz,
      textureOffset(tex_normal, in_uv, ivec2(0, 1)).xyz,
      textureOffset(tex_normal, in_uv, ivec2(1, 0)).xyz,
      textureOffset(tex_normal, in_uv, ivec2(1, 1)).xyz);

   /*
   float best_depth = max(max(d[0], d[1]), max(d[2], d[3]));
   float best_depth = mix(max(max(d[0], d[1]), max(d[2], d[3])),
                          min(min(d[0], d[1]), min(d[2], d[3])),
                          checkerboard(in_uv));
                          */
   float best_depth = most_representative(tex_depth, in_uv);

   for (int i = 0; i < 4; i++) {
      if (best_depth == d[i]) {
         out_color = vec4(n[i], d[i]);
         return;
      }
   }
}
