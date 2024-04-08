#version 330 core
//Returns a Color
out vec4 FragColor;

//Take in a vec3 textCoord
in vec3 texCoord;

//Add in our cubemap for later
uniform samplerCube skybox;

uniform vec3 skyBoxColor;

void main(){
	FragColor = texture(skybox, texCoord) + vec4(skyBoxColor, 1.0);
}