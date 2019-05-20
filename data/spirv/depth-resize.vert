#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec2 out_uv;

/*
const vec2 vdata[] = vec2[] (
      vec2(0.0, 0.0),
      vec2(0.0, 1.0),
      vec2(1.0, 1.0),
      vec2(1.0, 0.0));

void main()
{
   out_uv = vdata[gl_VertexIndex];
   out_position = vec4(vdata[gl_VertexIndex] * 2.0 - 1.0, 0.0, 1.0);
} */

void main()
{
   switch (gl_VertexIndex) {
   case 0:
      out_position = vec4( 1.0,  1.0, 0.0, 1.0);
      out_uv = vec2(1.0, 1.0);
      break;
   case 1:
      out_position = vec4( 1.0, -1.0, 0.0, 1.0);
      out_uv = vec2(1.0, 0.0);
      break;
   case 2:
      out_position = vec4(-1.0,  1.0, 0.0, 1.0);
      out_uv = vec2(0.0, 1.0);
      break;
   case 3:
      out_position = vec4(-1.0, -1.0, 0.0, 1.0);
      out_uv = vec2(0.0, 0.0);
      break;
   }
}
