#version 430
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 out_uv;

const vec2 vdata[] = vec2[] (
      vec2(1.0, 1.0),
      vec2(1.0, 0.0),
      vec2(0.0, 1.0),
      vec2(0.0, 0.0));

void main()
{
   out_uv = vdata[gl_VertexIndex];
   gl_Position = vec4(vdata[gl_VertexIndex] * 2.0 - 1.0, 0.0, 1.0);
}
