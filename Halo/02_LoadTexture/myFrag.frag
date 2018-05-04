#version 130

in vec3 vs_out_col;
in vec3 vs_out_pos;
out vec4 fs_out_col;

uniform sampler2D texImage;
uniform sampler2D texImage2;
uniform float t;

void main()
{
	fs_out_col = vec4(vs_out_col,1);
}