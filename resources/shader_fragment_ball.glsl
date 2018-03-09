#version 330 core
out vec3 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec4 gl_FragCoord;
uniform vec4 Center;
void main()
{
	color = vec3(255, 255, 255);
}
