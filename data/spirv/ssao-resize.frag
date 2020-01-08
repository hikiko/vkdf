#version 460
#extension GL_ARB_separate_shader_objects : enable

float checkerboard(in vec2 uv)
{
   vec2 pos = floor(uv);
   return mod(pos.x + mod(pos.y, 2.0), 2.0);
}

float most_representative(sampler2D tex_depth, vec2 in_uv)
{
   float samples[] = float[] (
      textureOffset(tex_depth, in_uv, ivec2(0, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(0, 1)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 0)).x,
      textureOffset(tex_depth, in_uv, ivec2(1, 1)).x);

   float centr = (samples[0] + samples[1] + samples[2] + samples[3]) / 4.0;
   float dist[] = float[] (
         abs(centr - samples[0]),
         abs(centr - samples[1]),
         abs(centr - samples[2]),
         abs(centr - samples[3]));
   
   float max_dist = max(max(dist[0], dist[1]), max(dist[2], dist[3]));
   float rem_samples[3];
   int rejected_idx[3];

   int j = 0; int i; int k = 0;
   for (i = 0; i < 4; i++) {
      if (dist[i] < max_dist) {
         rem_samples[j] = samples[i];
         j++;
      } else {
         /* for the extreme case where 2 or more samples
            have max_dist distance from the centroid */
         rejected_idx[k] = i;
         k++;
      }
   }

   /* also for the extreme case where 2 or more samples
      have max_dist distance from the centroid */
   if (j < 2) {
      for (i = 3; i > j; i--) {
         rem_samples[i] = samples[rejected_idx[k]];
         k--;
      }
   }

   centr = (rem_samples[0] + rem_samples[1] + rem_samples[2]) / 3.0;

   dist[0] = abs(rem_samples[0] - centr);
   dist[1] = abs(rem_samples[1] - centr);
   dist[2] = abs(rem_samples[2] - centr);

   float min_dist = min(dist[0], min(dist[1], dist[2]));
   for (int i = 0; i < 3; i++) {
      if (dist[i] == min_dist)
         return rem_samples[i];
   }

   /* should never reach */
   return samples[0];
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
