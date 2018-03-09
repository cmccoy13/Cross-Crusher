#version 330 core
out vec3 color;
in vec3 vertex_color;

in vec4 gl_FragCoord;

void main()
{
    color = vec3(0, 0, 0);

	color = vertex_color;

	/*
	if(gl_FragCoord.x <= 586)
	{
		color += vec3(255, 0, 0);
	}
	if(gl_FragCoord.x >= 914)
	{
		color += vec3(0, 255, 0);
	}
	if(gl_FragCoord.y <= 336)
	{
		color += vec3(0, 0, 255);
	}
	if(gl_FragCoord.y >= 664)
	{
		color += vec3(255, 255, 255);
	}
	*/
	
}
