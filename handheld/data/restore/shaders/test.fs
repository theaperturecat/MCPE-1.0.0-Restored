out vec4 FragColor;
in vec3 color;
in vec3 fPos;

void main()
{

FragColor = vec4(color,1.0);
//vec3 cell = floor(fPos + 0.001);

//float checker = mod(cell.x + cell.y + cell.z, 2.0);

//FragColor = vec4(
//    mix(vec3(0.0), color, checker),//vec3(1.0,0.0,0.0)
//    1.0
//);
//FragColor = vec4(normalize(fPos) * 0.5 + 0.5, 1.0);
}
