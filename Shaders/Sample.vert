#version 330 core

//Gets the data at Attrib Index 0
//Converts it and stores it into a Vec3
layout(location = 0) in vec3 aPos;

//The normals has attribute position 1
//Accesses the normalsa annd assigns it to vertexNormal
layout(location = 1) in vec3 vertexNormal;

//Pass the Processed normals to the fragment shader later
out vec3 normCoord;
//Pass the position of the vertex to the fragment shader later
out vec3 fragPos;

//The Tex coord /UV is at 2
layout(location = 2 ) in vec2 aTex;

//Location 3 == Tangent
layout(location = 3) in vec3 m_tan;

//Locatopn 4 == Bitangent
layout(location = 4) in vec3 m_btan;

//This is the martrix that will
//Convert tangent space to object space
out mat3 TBN;

//Pass the tex coord to the fragment shader
out vec2 texCoord;

//Declare a variable to hold the data
//that we're going to pass
//uniform float x;
//uniform float y;

//Creates a Transform variable
//We'll assign the transformation matrix here later
uniform mat4 transform;

//Projection Mat
uniform mat4 projection;

//Camera View Matrix
uniform mat4 view;

void main(){
	//Create a new vec3 for the new Position
	//					//Add x to aPos.x
	//vec3 newPos = vec3(aPos.x + x, aPos.y + y, aPos.z);

	//gl_Position is predefined
	//this denotes the final position of the vertex / point
	//gl_Position = vec4(newPos, 1.0); //Tuurns vex3 into a vec4

	//Multiply the transformation matrix to the
	//vec4
	gl_Position = projection * //Multiply the Projection Matrix with the view
					view *		//Multiply the View with the Position
					transform * //Multiply the matrix with the position
					vec4(aPos, 1.0); //Turns vex3 into a vec4

	//normCoord = mat3(Get the Normmal Matrix and convert it to a 3x3 matrix
					//transpose(inverse(transform))
					//) * vertexNormal; Apply the normal matrix to the normal data 

	mat3 modelMat = mat3(transpose(inverse(transform)));
	normCoord = modelMat * vertexNormal;

	vec3 T = normalize(modelMat * m_tan);
	vec3 B = normalize(modelMat * m_btan);
	vec3 N = normalize(normCoord);
	TBN = mat3(T, B, N);

	//The position is just your transfom matrix
	//applied to the vertex as a vector 3
	fragPos = vec3(transform * vec4(aPos,1.0));

	//Assign the UV
	texCoord = aTex;
}