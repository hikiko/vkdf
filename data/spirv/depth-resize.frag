#version 450
//#extension GL_ARB_separate_shader_objects : enable

/*layout(location = 0) in vec4 in_position;
layout(location = 1) in vec2 in_uv;

//layout(set = 0, binding = 0) uniform sampler2D tex_depth;
//layout (depth_any) out float gl_FragDepth;
*/
layout(location = 0) out vec4 out_color;
void main()
{
/*	float d1 = textureOffset(tex_depth, in_uv, ivec2(0, 0)).x;
	float d2 = textureOffset(tex_depth, in_uv, ivec2(0, 1)).x;
	float d3 = textureOffset(tex_depth, in_uv, ivec2(1, 1)).x;
	float d4 = textureOffset(tex_depth, in_uv, ivec2(1, 0)).x;
*/
	/*
	   for the moment we always take the max, then we replace it
	   with once min once max, depending on the pattern we need to
	   check
	 */
	gl_FragDepth = 0.5;//max(max(d1, d2), max(d3, d4));
   out_color = vec4(1.0, 0.2, 0.1, 1.0);
}
