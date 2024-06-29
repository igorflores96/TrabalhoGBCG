#shader vertex
#version 330 core

layout (location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout (location = 2) in vec2 texc;

out vec4 FinalColor;
out vec2 TexCoord;

uniform mat4 u_Projection;

void main()
{
   gl_Position = u_Projection * position;
   FinalColor = color;
   TexCoord = texc;
};


#shader fragment
#version 330 core

in vec4 FinalColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texbuffer;

void main()
{
    vec4 texColor = texture(texbuffer, TexCoord);
    FragColor = FinalColor; // * texColor
};