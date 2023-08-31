#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;


out vec4 finalColor;

// float numColors = 32.0;
float numColors = 15.0;

void main()
{

    vec3 texelColor = texture(texture0, fragTexCoord.xy).rgb;

    texelColor = texelColor*numColors;
    texelColor = floor(texelColor);
    texelColor = texelColor/numColors;

    finalColor = vec4(texelColor, 1.0);
}