layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aColor;
layout (location = 2) in int aNormal;
layout (location = 3) in vec2 aUV0;
layout (location = 4) in vec2 aUV1;
layout (location = 5) in vec2 aUV2;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 color;
out vec3 fPos;
out vec2 mUV;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

uint colorl = aColor;
fPos = vec3(model * vec4(aPos, 1.0));

mUV = aUV0;

color = vec3(
    float((colorl >> 16) & 0xFFu), // R
    float((colorl >>  8) & 0xFFu), // G
    float(colorl         & 0xFFu) // B
    //float((colorl >> 24) & 0xFFu)  // A
) / 255.0;
}