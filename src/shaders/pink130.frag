#version 120

uniform sampler2D colorIn;
uniform sampler2D palette;
in vec2 texCoord;

void main()
{
	vec4 idx = texture(colorIn, gl_TexCoord[0].st);
	vec4 colorOut = texture(palette, vec2(idx.w, 0.0));
	gl_FragColor = colorOut;
}

