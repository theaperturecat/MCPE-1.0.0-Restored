out vec4 FragColor;
in vec3 color;
in vec3 fPos;
in vec2 mUV;

uniform sampler2D atlas;



void main()
{
vec4 bcolor = texture(atlas, mUV) * vec4(color,1.0);
FragColor = bcolor;//vec4(mUV.x,mUV.y,1.0,1.0);//color;//vec4(color,1.0);
//vec3 cell = floor(fPos + 0.001);

//float checker = mod(cell.x + cell.y + cell.z, 2.0);

//FragColor = vec4(
//    mix(vec3(0.0), color, checker),//vec3(1.0,0.0,0.0)
//    1.0
//);
//FragColor = vec4(normalize(fPos) * 0.5 + 0.5, 1.0);
}
