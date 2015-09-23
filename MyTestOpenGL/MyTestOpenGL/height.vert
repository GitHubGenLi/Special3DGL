attribute float height;

void main()
{	
	vec4 p;

	p.xz = gl_Vertex.xy;
	p.y = height;
	p.w = 1.0;

	gl_Position = gl_ModelViewProjectionMatrix * p;
}
