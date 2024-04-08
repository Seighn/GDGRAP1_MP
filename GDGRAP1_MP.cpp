#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Modifier for the model's x Position
float x_mod = 0;
float y_mod = 0;
float z_mod = 0;

//Variables for mouse input
float lastX = 400, lastY = 300;
bool firstMouse = true;
float yaw = -90.f;
float pitch = 0.f;

//Movement Flags
bool up = false;
bool down = false;
bool left = false;
bool right = false;
bool ascend = false;
bool descend = false;

//Orthograpic Movement Flags
bool upDirection = false;
bool downDirection = false;
bool leftDirection = false;
bool rightDirection = false;


//Camera keys
//For Perspective Projection
bool changePerspective = false;

//Toggle for Ortho and Perspective
bool changeProjection = false;

//Cycle Light Intensity
bool low = true;
bool medium = false;
bool high = false;

void Key_Callback(GLFWwindow* window, // the pointer to the window
    int key, // the keycode being pressed
    int scancode, // Physical position of the press on keyboard
    int action, // Either Press / Release
    int mods) //Which modifier keys is held down
{
    ////When user pressed D
    if (key == GLFW_KEY_RIGHT)
        x_mod += 10.f;
    //if (key == GLFW_KEY_W)
    //    z_mod += 0.5f;
    if (key == GLFW_KEY_LEFT)
        x_mod -= 10.f;
    //if (key == GLFW_KEY_S)
    //    z_mod -= 0.5f;
    //Backward
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS)
            down = true;
        else if (action == GLFW_RELEASE)
            down = false;
    }
    //Forward
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS)
            up = true;
        else if (action == GLFW_RELEASE)
            up = false;
    }

    //Left
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS)
            left = true;
        else if (action == GLFW_RELEASE)
            left = false;
    }

    //Right
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS)
            right = true;
        else if (action == GLFW_RELEASE)
            right = false;
    }

    //Ascend
    if (key == GLFW_KEY_Q) {
        if (action == GLFW_PRESS)
            ascend = true;
        else if (action == GLFW_RELEASE)
            ascend = false;
    }

    //Descend
    if (key == GLFW_KEY_E) {
        if (action == GLFW_PRESS)
            descend = true;
        else if (action == GLFW_RELEASE)
            descend = false;
    }

    //Move Down for Orthographic
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS)
            downDirection = true;
        else if (action == GLFW_RELEASE)
            downDirection = false;
    }
    //Move Up for Orthographic
    if (key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS)
            upDirection = true;
        else if (action == GLFW_RELEASE)
            upDirection = false;
    }

    //Left for Orthographic
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS)
            leftDirection = true;
        else if (action == GLFW_RELEASE)
            leftDirection = false;
    }

    //Right for Orthographic
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS)
            rightDirection = true;
        else if (action == GLFW_RELEASE)
            rightDirection = false;
    }

    //Change 3rd Person Perspective to 1st Person
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        if (changePerspective == true)
            changePerspective = false;
        else
            changePerspective = true;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        if (changeProjection == true)
            changeProjection = false;
        else
            changeProjection = true;
    }

    //Cycle Light Intensity
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        if (low) {
            low = false;
            medium = true;
        }
        else if (medium) {
            medium = false;
            high = true;
        }
        else if (high) {
            high = false;
            low = true;
        }
    }

}

//Call Mouse
//Source::learnopengl.com/Getting-started/Camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

class Light;

//Create Shader
class Shader {
private:

    //Shaders
    GLuint vertexShader;
    GLuint fragShader;
    GLuint shaderProg;
public:
    //read vertex and frag shader file
    Shader(const char* v, const char* f){
        this->compileShaders(v, f);
    }


    //Compile vertex and frag shaders into one
    void compileShaders(const char* v, const char* f) {
        //Create a Vertex Shader
        this->vertexShader = glCreateShader(GL_VERTEX_SHADER);

        //Assign the source to the Vertex Shader
        glShaderSource(this->vertexShader, 1, &v, NULL);

        //Compile the Vertex Shader
        glCompileShader(this->vertexShader);

        //Create a Fragment Shader
        this->fragShader = glCreateShader(GL_FRAGMENT_SHADER);

        //Assign the source to the Fragment Shader
        glShaderSource(this->fragShader, 1, &f, NULL);

        //Compile the Fragment Shader
        glCompileShader(this->fragShader);

        //Create the Shader Program
        this->shaderProg = glCreateProgram();
        //Attach the compiled Vertex Shader
        glAttachShader(this->shaderProg, this->vertexShader);
        //Attach the compiled Fragment Shader
        glAttachShader(this->shaderProg, this->fragShader);

        //Finalize the compilation process
        glLinkProgram(this->shaderProg);

        glDeleteShader(this->vertexShader);
        glDeleteShader(this->fragShader);
    }

    GLuint getShaderProg() {
        return this->shaderProg;
    }

};

//Create Skybox
class SkyBox {
private:
    /*
  7--------6
 /|       /|
4--------5 |
| |      | |
| 3------|-2
|/       |/
0--------1
*/
//Vertices for the cube
    float skyboxVertices[24];

    //Skybox Indices
    unsigned int skyboxIndices[36];

    //Skybox VAO,VBO,EBO
    unsigned int 
        skyboxVAO, skyboxVBO, skyboxEBO;

    //Skybox Cube images
    std::string facesSkybox[6];

    //Skybox texture
    unsigned int skyboxTex;

    //Initialize the SkyBox's view matrix
    glm::mat4 sky_view = glm::mat4(1.0);

    //Skybox shader
    GLuint skyBoxShader;

    //SkyBox Color
    GLuint skyBoxColorAddress;


public:
    //Constructor of Skybox
    //Initialize the vertices, indices, Shader and image to use
    SkyBox(GLuint skyBoxShader,
        std::string img_1, std::string img_2, std::string img_3, 
        std::string img_4, std::string img_5, std::string img_6) 
    {
        this->skyboxVertices[0] = - 1.f;    this->skyboxVertices[1] = -1.f;     this->skyboxVertices[2] = 1.f;   //0
        this->skyboxVertices[3] = 1.f;      this->skyboxVertices[4] = -1.f;     this->skyboxVertices[5] = 1.f;   //1
        this->skyboxVertices[6] = 1.f;      this->skyboxVertices[7] = -1.f;     this->skyboxVertices[8] = -1.f;  //2
        this->skyboxVertices[9] = -1.f;     this->skyboxVertices[10] = -1.f;    this->skyboxVertices[11] = -1.f; //3
        this->skyboxVertices[12] = -1.f;    this->skyboxVertices[13] = 1.f;     this->skyboxVertices[14] = 1.f;  //4
        this->skyboxVertices[15] = 1.f;     this->skyboxVertices[16] = 1.f;     this->skyboxVertices[17] = 1.f;  //5
        this->skyboxVertices[18] = 1.f;     this->skyboxVertices[19] = 1.f;     this->skyboxVertices[20] = -1.f; //6
        this->skyboxVertices[21] = -1.f;    this->skyboxVertices[22] = 1.f;     this->skyboxVertices[23] = -1.f; //7

        this->skyboxIndices[0] = 1; this->skyboxIndices[1] = 2; this->skyboxIndices[2] = 6;
        this->skyboxIndices[3] = 6; this->skyboxIndices[4] = 5; this->skyboxIndices[5] = 1;

        this->skyboxIndices[6] = 7; this->skyboxIndices[7] = 4; this->skyboxIndices[8] = 7;
        this->skyboxIndices[9] = 7; this->skyboxIndices[10] = 3; this->skyboxIndices[11] = 0;

        this->skyboxIndices[12] = 4; this->skyboxIndices[13] = 5; this->skyboxIndices[14] = 6;
        this->skyboxIndices[15] = 6; this->skyboxIndices[16] = 7; this->skyboxIndices[17] = 4;

        this->skyboxIndices[18] = 0; this->skyboxIndices[19] = 3; this->skyboxIndices[20] = 2;
        this->skyboxIndices[21] = 2; this->skyboxIndices[22] = 1; this->skyboxIndices[23] = 0;

        this->skyboxIndices[24] = 0; this->skyboxIndices[25] = 1; this->skyboxIndices[26] = 5;
        this->skyboxIndices[27] = 5; this->skyboxIndices[28] = 4; this->skyboxIndices[29] = 0;

        this->skyboxIndices[30] = 3; this->skyboxIndices[31] = 7; this->skyboxIndices[32] = 6;
        this->skyboxIndices[33] = 6; this->skyboxIndices[34] = 2; this->skyboxIndices[35] = 3;

        this->facesSkybox[0] = img_1;
        this->facesSkybox[1] = img_2;
        this->facesSkybox[2] = img_3;
        this->facesSkybox[3] = img_4;
        this->facesSkybox[4] = img_5;
        this->facesSkybox[5] = img_6;

        this->skyBoxShader = skyBoxShader;
        
    }

    void createSkyBox() {
        //Generete Skybox Vertex Array and Buffers
        glGenVertexArrays(1, &this->skyboxVAO);
        glGenBuffers(1, &this->skyboxVBO);
        glGenBuffers(1, &this->skyboxEBO);

        //Bind Array and Buffer
        glBindVertexArray(this->skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &this->skyboxVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->skyboxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &this->skyboxIndices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);

        //Generate our skybox texture
        glGenTextures(1, &this->skyboxTex);
        //initialize it as a cube map
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTex);
        //this is to avoid out skybox from pixelating when its too big / small
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //Instead of ST only- cubemaps rely on RST
        //This is to make sure the texture stretched to the edge
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        for (unsigned int i = 0; i < 6; i++) {
            int w, h, skyCChannel;

            stbi_set_flip_vertically_on_load(false);

            unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

            if (data) {

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGBA,
                    w,
                    h,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    data
                );

                stbi_image_free(data);
            }
        }

        stbi_set_flip_vertically_on_load(true);
    }

    void generateSkyboxView(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 skyBoxColor) {

        this->sky_view = glm::mat4(glm::mat3(viewMatrix));


        this->skyBoxColorAddress = glGetUniformLocation(this->skyBoxShader, "skyBoxColor");
        glUniform3fv(this->skyBoxColorAddress, 1, glm::value_ptr(skyBoxColor));

        unsigned int skyboxViewLoc = glGetUniformLocation(this->skyBoxShader, "view");

        glUniformMatrix4fv(skyboxViewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(this->sky_view)); //View Matrix

        unsigned int skyboxProjLoc = glGetUniformLocation(this->skyBoxShader, "projection");

        glUniformMatrix4fv(skyboxProjLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projectionMatrix));

        glBindVertexArray(this->skyboxVAO);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }
};

//Create Model
class Model3D {

    //Fields for Model
private:

    int img_width, //Width of the texture
        img_height, //Height of the texture
        colorChannels; //Number of color channels
    unsigned char* tex_bytes; // Tex_bytes

    int img_width2, //Width of the texture
        img_height2, //Height of the texture
        colorChannels2; //Number of color channels
    unsigned char* normal_bytes; // Normal_Tex_bytes

    //Shaders
    GLuint texture;
    GLuint norm_tex;
    GLuint vertexShader;
    GLuint fragShader;
    GLuint shaderProg;

    //Texture Address
    GLuint tex0Address;
    GLuint tex1Address;

    //Obj file attributes
    std::string path;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;
    bool success;

    //Vertex Data
    std::vector<GLuint> mesh_indices;
    std::vector<GLfloat> fullVertexData;

    //Normal Mapping
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    //VertexArrayObject and VertexBufferObject
    GLuint VAO, VBO;

    //Matrices
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);
    glm::mat4 transformation_matrix;

    //Position of the Model
    glm::vec3 modelPosition;

public:
    //Constructor & Destructor
    Model3D(std::string image, std::string obj) {
        //Texture
        stbi_set_flip_vertically_on_load(true);

        this->tex_bytes =
            stbi_load(image.c_str(), //Texture path
                &this->img_width, //Fills out the width
                &this->img_height, //Fills out the height
                &this->colorChannels, //Fills out the color channels
                0);

        //Obj
        this->path = obj.c_str();

        this->success = tinyobj::LoadObj(
            &this->attributes,
            &this->shapes,
            &this->material,
            &this->warning,
            &this->error,
            this->path.c_str()
        );
    }

    Model3D(std::string image, std::string normal_img, std::string obj) {
        //Texture
        stbi_set_flip_vertically_on_load(true);

        this->tex_bytes =
            stbi_load(image.c_str(), //Texture path
                &this->img_width, //Fills out the width
                &this->img_height, //Fills out the height
                &this->colorChannels, //Fills out the color channels
                0);

        //Obj
        this->path = obj.c_str();

        this->success = tinyobj::LoadObj(
            &this->attributes,
            &this->shapes,
            &this->material,
            &this->warning,
            &this->error,
            this->path.c_str()
        );

        //Normal Texture
        stbi_set_flip_vertically_on_load(true);

        this->normal_bytes =
            stbi_load(normal_img.c_str(), //Texture path
                &this->img_width2, //Fills out the width
                &this->img_height2, //Fills out the height
                &this->colorChannels2, //Fills out the color channels
                0);

    }

    //Delete Vertex Object
    ~Model3D() {
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);
    }

    //Methods
public:
    //read vertex and frag shader file
    void setShaders(GLuint shaderProg) {
        this->shaderProg = shaderProg;
    }

private:
    //Generate textures
    void createTexture() {
        //Generate reference
        glGenTextures(1, &this->texture);
        //Set the current texture we're
        //working
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, this->texture);


        //Assign the loaded teexture
        //to the OpenGL reference
        if (this->colorChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D,
                0, //Texture 0
                GL_RGB, // Target color format of the texture
                this->img_width, // Texture width
                this->img_height,// Texture height
                0,
                GL_RGB,    //Color format of the texturue
                GL_UNSIGNED_BYTE, //Data type of texture
                this->tex_bytes); // Loaded texture in bytes
        }
        else{
            glTexImage2D(GL_TEXTURE_2D,
                0, //Texture 0
                GL_RGBA, // Target color format of the texture
                this->img_width, // Texture width
                this->img_height,// Texture height
                0,
                GL_RGBA,    //Color format of the texturue
                GL_UNSIGNED_BYTE, //Data type of texture
                this->tex_bytes); // Loaded texture in bytes
        }

        //Generate thhe mipmaps to the current texture
        glGenerateMipmap(GL_TEXTURE_2D);

        //Free uo the loaded bytes
        stbi_image_free(this->tex_bytes);
    }

    void createNormalTexture(){
        glGenTextures(1, &this->norm_tex);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->norm_tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        //Assign the loaded texture
        //too the OpenGL reference
        if(this->colorChannels2 == 3){
            glTexImage2D(GL_TEXTURE_2D,
                0, //Texture 0
                GL_RGB, // Target color format of the texture
                this->img_width2, // Texture width
                this->img_height2,// Texture height
                0,
                GL_RGB,    //Color format of the texturue
                GL_UNSIGNED_BYTE, //Data type of texture
                this->normal_bytes); // Loaded texture in bytes
        }
        else {
            glTexImage2D(GL_TEXTURE_2D,
                0, //Texture 0
                GL_RGBA, // Target color format of the texture
                this->img_width2, // Texture width
                this->img_height2,// Texture height
                0,
                GL_RGBA,    //Color format of the texturue
                GL_UNSIGNED_BYTE, //Data type of texture
                this->normal_bytes); // Loaded texture in bytes
        }

        //Generate thhe mipmaps to the current texture
        glGenerateMipmap(GL_TEXTURE_2D);
 
        //Free up the loaded bytes
        stbi_image_free(this->normal_bytes);
    }

    //set the Vertex and texture data of the object
    void setVertAndTex() {
        for (int i = 0; i < this->shapes[0].mesh.indices.size(); i++) {
            this->mesh_indices.push_back(
                this->shapes[0].mesh.indices[i].vertex_index
            );
        }

        for (int i = 0; i < this->shapes[0].mesh.indices.size(); i += 3) {
            tinyobj::index_t vData1 = this->shapes[0].mesh.indices[i];
            tinyobj::index_t vData2 = this->shapes[0].mesh.indices[i + 1];
            tinyobj::index_t vData3 = this->shapes[0].mesh.indices[i + 2];

            glm::vec3 v1 = glm::vec3(
                this->attributes.vertices[vData1.vertex_index * 3],
                this->attributes.vertices[(vData1.vertex_index * 3) + 1],
                this->attributes.vertices[(vData1.vertex_index * 3) + 2]
            );
            glm::vec3 v2 = glm::vec3(
                this->attributes.vertices[vData2.vertex_index * 3],
                this->attributes.vertices[(vData2.vertex_index * 3) + 1],
                this->attributes.vertices[(vData2.vertex_index * 3) + 2]
            );
            glm::vec3 v3 = glm::vec3(
                this->attributes.vertices[vData3.vertex_index * 3],
                this->attributes.vertices[(vData3.vertex_index * 3) + 1],
                this->attributes.vertices[(vData3.vertex_index * 3) + 2]
            );

            glm::vec2 uv1 = glm::vec2(
                this->attributes.texcoords[vData1.texcoord_index * 2],
                this->attributes.texcoords[(vData1.texcoord_index * 2) + 1]
            );
            glm::vec2 uv2 = glm::vec2(
                this->attributes.texcoords[vData2.texcoord_index * 2],
                this->attributes.texcoords[(vData2.texcoord_index * 2) + 1]
            );
            glm::vec2 uv3 = glm::vec2(
                this->attributes.texcoords[vData3.texcoord_index * 2],
                this->attributes.texcoords[(vData3.texcoord_index * 2) + 1]
            );

            glm::vec3 deltaPos1 = v2 - v1;
            glm::vec3 deltaPos2 = v3 - v1;

            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            for (int i = 0; i < 3; i++) {
                this->tangents.push_back(tangent);
                this->bitangents.push_back(bitangent);
            }


        }

        for (int i = 0; i < this->shapes[0].mesh.indices.size(); i++) {

            //Assign the Index data for easy access
            tinyobj::index_t vData = this->shapes[0].mesh.indices[i];

            //Push the X position of the vertex
            this->fullVertexData.push_back(
                //Multiply the index by 3 to get the base offset
                this->attributes.vertices[(vData.vertex_index * 3)]
            );

            //Push the Y position of the vertex
            this->fullVertexData.push_back(
                //Add the base offset to 1 to get Y
                this->attributes.vertices[(vData.vertex_index * 3) + 1]
            );

            //Push the Z position of the vertex
            this->fullVertexData.push_back(
                //Add the base offset to 2 to get Z
                this->attributes.vertices[(vData.vertex_index * 3) + 2]
            );

            this->fullVertexData.push_back(
                //Add the base offset to 2 to get Z
                this->attributes.normals[(vData.normal_index * 3)]
            );

            this->fullVertexData.push_back(
                //Add the base offset to 2 to get Z
                this->attributes.normals[(vData.normal_index * 3) + 1]
            );

            this->fullVertexData.push_back(
                //Add the base offset to 2 to get Z
                attributes.normals[(vData.normal_index * 3) + 2]
            );

            //Push the U of the Tex Coords
            this->fullVertexData.push_back(
                //Multiply the index by 3 to get the base offset
                this->attributes.texcoords[(vData.texcoord_index * 2)]
            );

            //Push the V of the Tex Coords
            this->fullVertexData.push_back(
                //Add the base offset to 1 to get V
                this->attributes.texcoords[(vData.texcoord_index * 2) + 1]
            );

            this->fullVertexData.push_back(
                this->tangents[i].x
            );
            this->fullVertexData.push_back(
                this->tangents[i].y
            );
            this->fullVertexData.push_back(
                this->tangents[i].z
            );
            this->fullVertexData.push_back(
                this->bitangents[i].x
            );
            this->fullVertexData.push_back(
                this->bitangents[i].y
            );
            this->fullVertexData.push_back(
                this->bitangents[i].z
            );


        }
    }

public:
    //Createing the model
    void createModel() {
        //Call the neccessary functions to create model
        this->createTexture();
        this->createNormalTexture();
        this->setVertAndTex();

        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);

        //Generate VAO
        glGenVertexArrays(1, &this->VAO);

        //Generate VBO
        glGenBuffers(1, &this->VBO);


        //Bind VAO and VBO
        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

        //Position
        glBufferData(
            GL_ARRAY_BUFFER,
            //Size of the whole array in bytes
            sizeof(GLfloat) * this->fullVertexData.size(),
            //Data of the array
            this->fullVertexData.data(),
            GL_DYNAMIC_DRAW
        );

        glVertexAttribPointer(
            0, //index 0 is the vertex position
            3, //Position is 3 floats (x,y,z)
            GL_FLOAT, // Data type of array
            GL_FALSE,

            //Our vertex data has 8 floats in it
            //(X,Y,Z,Normals,U,V)
            14 * sizeof(GLfloat),//size of the vertex data in bytes
            (void*)0
        );

        glEnableVertexAttribArray(0);

        //Since our UV starts at index 3
        //or the 4th of our index data

        //Normalize
        GLintptr normalPtr = 3 * sizeof(float);

        glVertexAttribPointer(
            1, //index 0 is the vertex position
            3, //Position is 3 floats (x,y,z)
            GL_FLOAT, // Data type of array
            GL_FALSE,

            //Our vertex data has 8 floats in it
            //(X,Y,Z,Normals,U,V)
            14 * sizeof(GLfloat),//size of the vertex data in bytes
            (void*)normalPtr
        );

        glEnableVertexAttribArray(1);


        //UV
        GLintptr uvPtr = 6 * sizeof(float);

        glVertexAttribPointer(
            2, //index 0 is the vertex position
            2, //Position is 3 floats (x,,z)
            GL_FLOAT, // Data type of array
            GL_FALSE,

            //Our vertex data has 8 floats in it
            //(X,Y,Z,Normals,U,V)
            14 * sizeof(GLfloat),//size of the vertex data in bytes
            (void*)uvPtr
        );
        glEnableVertexAttribArray(2);

        //Tangent
        GLintptr tangentPtr = 8 * sizeof(float);

        glVertexAttribPointer(
            3, //index 3 is the tangent
            3, //Position is 3 floats (x,y,z)
            GL_FLOAT, // Data type of array
            GL_FALSE,

            //Our vertex data has 8 floats in it
            //(X,Y,Z,U,V)
            14 * sizeof(GLfloat),//size of the vertex data in bytes
            (void*)tangentPtr
        );
        glEnableVertexAttribArray(3);

        //Tangent
        GLintptr bitangentPtr = 11 * sizeof(float);

        glVertexAttribPointer(
            4, //index 4 is the bitangent
            3, //Position is 3 floats (x,y,z)
            GL_FLOAT, // Data type of array
            GL_FALSE,

            //Our vertex data has 8 floats in it
            //(X,Y,Z,U,V)
            14 * sizeof(GLfloat),//size of the vertex data in bytes
            (void*)bitangentPtr
        );
        glEnableVertexAttribArray(4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Currently editing VBO = null

        //Currently editing VAO = VAO
        glBindVertexArray(0);
        //Currently editing VAO = null

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    //Set Object Position
    void updateTranslate(float translate_x, float translate_y, float translate_z) {
        if (translate_y > 0) {
            translate_y = 0;
        }

        this->transformation_matrix =
            glm::translate(this->identity_matrix4,
                glm::vec3(translate_x, translate_y, translate_z)
            );
        //Current Position of model
        this->modelPosition = glm::vec3(this->transformation_matrix[3]);
    }
    //Scale the Object
    void updateScale(float scale_x, float scale_y, float scale_z) {
        this->transformation_matrix =
            glm::scale(this->transformation_matrix,
                glm::vec3(scale_x, scale_y, scale_z)
            );
    }

    //Rotate the Object
    void updateRotation(float rotate_x, float rotate_y, float rotate_z) {
        //X Rotation
        this->transformation_matrix =
            glm::rotate(this->transformation_matrix,
                glm::radians(rotate_x),
                glm::normalize(glm::vec3(1.f, 0.f, 0.f))
            );
        //Y Rotation
        this->transformation_matrix =
            glm::rotate(this->transformation_matrix,
                glm::radians(rotate_y),
                glm::normalize(glm::vec3(0.f, 1.f, 0.f))
            );
        //Z Rotation
        this->transformation_matrix =
            glm::rotate(this->transformation_matrix,
                glm::radians(rotate_z),
                glm::normalize(glm::vec3(0.f, 0.f, 1.f))
            );
    }

    //Revolve object around a center
   /* void updateRevolution(float revolve_x, float revolve_y, float revolve_z, float rotate_x,
        float rotate_y, float rotate_z, PointLight* lightPos);*/

    //Updating Transformation matrix
    void update() {

        unsigned int transformLoc = glGetUniformLocation(this->shaderProg, "transform");

        glUniformMatrix4fv(transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(this->transformation_matrix));
    }
    //Render Texture with light
    void renderTexture();

    //Render the Complete object
    void perform() {
        //Update object
        this->update();
        //Render Texture
        this->renderTexture();


        glBindVertexArray(this->VAO);

        //Rendering the model
        glDrawArrays(GL_TRIANGLES, 0, this->fullVertexData.size() / 14);
    }

    //Getters
    GLuint getShaderProg() {
        return this->shaderProg;
    }

    glm::vec3 getModelPos() {
        return this->modelPosition;
    }
};

//Create Player
class Player {
private:
    Model3D* model;

public:
    //Constructor
    Player(std::string img, std::string normal, std::string obj, GLuint shader) {
        this->model = new Model3D(img,normal,obj);
        this->model->setShaders(shader);
        this->model->createModel();
    }

    //Render
    void perform() {
        this->model->perform();
    }

    //Setters and getters
    void setPosition(glm::vec3 Position) {
        this->model->updateTranslate(Position.x,Position.y,Position.z);
    }
    void setRotation(glm::vec3 Rotation) {
        this->model->updateRotation(Rotation.x, Rotation.y, Rotation.z);
    }
    void setScale(glm::vec3 Scale) {
        this->model->updateScale(Scale.x,Scale.y,Scale.z);
    }

    glm::vec3 getPlayerPos() {
        return this->model->getModelPos();
    }
};

//Create Camera Abstract Class
class MyCamera {
    //Camera Fields
protected:
    //Matrix for the projection
    glm::mat4 projectionMatrix;

    //The camera's poistion
    glm::vec3 cameraPos;
    glm::mat4 cameraPositionMatrix;

    //The camera's eye
    glm::vec3 WorldUp;
    glm::vec3 Center;
    glm::vec3 F;
    glm::vec3 R;
    glm::vec3 U;

    //The camera's orientation
    glm::mat4 cameraOrientation;

    //The View of the Camera
    glm::mat4 viewMatrix;

    //Window height and width
    float window_height;
    float window_width;

    //Default Constructor
public:
    MyCamera() {}


public:
    //Pure virtual function to implement projection type
    virtual void createProjection() = 0;
private:
    //Create the initial camera position
    void createCameraPos() {
        this->cameraPos = glm::vec3(0.f, 3.f, 10.f);

        //Construct the Position Matrix

        this->cameraPositionMatrix =
            glm::translate(glm::mat4(1.0f), //Intialize it as an Identity Matrix
                this->cameraPos * -1.f); //Multiply to -1 since we need -P

        //World's Up Direction
        //Normally just 1 in Y
        this->WorldUp = glm::vec3(0, 1.f, 0);

        //Camera's Center
        this->Center = glm::vec3(0.f, 1.f, 0.f);

        //Get Forward
        this->F = glm::vec3(this->Center - this->cameraPos);

        //Normalize the Forward
        this->F = glm::normalize(this->F);

        this->R = glm::normalize(
            //R x F
            glm::cross(this->F, WorldUp)
        );

        this->U = glm::normalize(
            //R x F
            glm::cross(this->R, this->F)
        );
    }

    //Create the initial camera orientation
    void createCameraOrientation() {
        //Construct the Camera Orientation Matrix
        this->cameraOrientation = glm::mat4(1.f);

        //Manually assign the Matrix
        //Matrix[Column][Row]
        this->cameraOrientation[0][0] = R.x;
        this->cameraOrientation[1][0] = R.y;
        this->cameraOrientation[2][0] = R.z;

        this->cameraOrientation[0][1] = U.x;
        this->cameraOrientation[1][1] = U.y;
        this->cameraOrientation[2][1] = U.z;

        this->cameraOrientation[0][2] = -F.x;
        this->cameraOrientation[1][2] = -F.y;
        this->cameraOrientation[2][2] = -F.z;
    }

    //Create the initial camera view
    void createCameraView() {
        //Camera View Matrix
        this->viewMatrix = cameraOrientation * cameraPositionMatrix;
    }
public:
    //Create the Camera
    void createCamera() {
        this->createProjection();
        this->createCameraPos();
        this->createCameraOrientation();
        this->createCameraView();
    }
public:
    //Render the camera
    void render(GLuint shaderProg) {

        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");

        glUniformMatrix4fv(projectionLoc,
            1,
            GL_FALSE,
            glm::value_ptr(this->projectionMatrix));

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");

        glUniformMatrix4fv(viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(this->viewMatrix)); //View Matrix
    }
    //Virtual function for children to edit
    virtual void perform(GLuint shaderProg) {
        this->render(shaderProg);
    }

    //Getters
    glm::vec3 getCameraPos() {
        return this->cameraPos;
    }

    glm::mat4 getViewMatrix() {
        return this->viewMatrix;
    }

    glm::mat4 getProjectionMatrix() {
        return this->projectionMatrix;
    }
    void setCameraPosition(glm::vec3 modelPos) {
        this->cameraPos = modelPos;
    }
};

//Create Camera with Orthographic Projection
class OrthoCamera :
    public MyCamera {
//Constructor
public:
    OrthoCamera(float window_height, float window_width) : MyCamera() {
        this->window_height = window_height;
        this->window_width = window_width;
    }

public:
    //Create Orthographic Projection
    void createProjection() {
        this->projectionMatrix = glm::ortho(-20.0f, //Left
            20.0f, //Right
            -20.0f, //Bottom
            20.0f, //Top
            -0.1f, //Z-Near
            1000.f); //Z-Far
    }
private:
    //New camera position
    void updateCameraPos() {
        this->cameraPos = glm::vec3(0.f, 20.f, 0.f);

        //float speed = 0.005f;
        //if(upDirection)
        //    this->cameraPos += speed * this->U;
        //if(downDirection)
        //    this->cameraPos -= speed * this->U;
        //if(rightDirection)
        //    this->cameraPos += glm::normalize(glm::cross(this->F, this->U)) * speed;
        //if(leftDirection)
        //    this->cameraPos -= glm::normalize(glm::cross(this->F, this->U)) * speed;

        //Construct the Position Matrix
        this->cameraPositionMatrix =
            glm::translate(glm::mat4(1.0f), //Intialize it as an Identity Matrix
                this->cameraPos * -1.f); //Multiply to -1 since we need -P

        //World's Up Direction
        this->WorldUp = glm::vec3(0.f, 0.f, 1.f);

        //Camera's Center
        this->Center = glm::vec3(0.f, 1.f, 0.f);

        //Get Forward
        this->F = glm::vec3(this->Center - this->cameraPos);

        //Normalize the Forward
        this->F = glm::normalize(this->F);

        this->R = glm::normalize(
            //R x F
            glm::cross(this->F, WorldUp)
        );

        this->U = glm::normalize(
            //R x F
            glm::cross(this->R, this->F)
        );
    }
    //Update view withh lookAt function
    //Source::learnopengl.com/Getting-started/Camera
    void updateViewMatrix() {
        //Using the lookAt function for easy calculation of camera orientation and camera position
        this->viewMatrix = glm::lookAt(this->cameraPos, this->cameraPos + this->F, this->U);
    }

    //Update function that performs all updates
    void update() {
        this->updateCameraPos();
        this->updateViewMatrix();
    }

public:
    //Perfrom camera
    void performCamera(GLuint shaderProg) {
        this->update();
        this->render(shaderProg);
    }

};

//Create Camera with Perspective Projection
class PerspectiveCamera :
    public MyCamera {
private:
    //Direction vector
    glm::vec3 direction;
public:
    //Constructor
    PerspectiveCamera(float window_height, float window_width) : MyCamera() {
        this->window_height = window_height;
        this->window_width = window_width;
    }

public:
    //Create Perpective Projection
    void createProjection() {
        this->projectionMatrix = glm::perspective(
            glm::radians(90.f), //FOV
            this->window_height / this->window_width, //Aspect ratio
            0.1f, //ZNear > 0
            30.f // ZFar
        );

    }

    void updateZfarView() {
        this->projectionMatrix = glm::perspective(
            glm::radians(90.f), //FOV
            this->window_height / this->window_width, //Aspect ratio
            0.1f, //ZNear > 0
            100.f // ZFar
        );
    }
private:
    //Update camera's position
    //Source::learnopengl.com/Getting-started/Camera
    void updateCameraPos() {
        float speed = 0.005f;

        //camera movement
        if (up)
            this->cameraPos += speed * this->F;
        if (down)
            this->cameraPos -= speed * this->F;
        if (left)
            this->cameraPos -= glm::normalize(glm::cross(this->F, this->U)) * speed;
        if (right)
            this->cameraPos += glm::normalize(glm::cross(this->F, this->U)) * speed;
        if (ascend)
            this->cameraPos += speed * this->U;
        if (descend)
            this->cameraPos -= speed * this->U;
        if (this->cameraPos.y > 5) {
            this->cameraPos.y = 5;
        }
        this->cameraPositionMatrix =
            glm::translate(glm::mat4(1.0f), //Intialize it as an Identity Matrix
                this->cameraPos * -1.f); //Multiply to -1 since we need -P


    }

    //Update camera's orientation
    //Source::learnopengl.com/Getting-started/Camera
    void updateCameraOrientation() {

        //Orientation of x,y,z
        this->direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->direction.y = sin(glm::radians(pitch));
        this->direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        //Normalize F
        this->F = glm::normalize(this->direction);


    }

    //Update view withh lookAt function
    //Source::learnopengl.com/Getting-started/Camera
    void updateViewMatrix() {
        //Using the lookAt function for easy calculation of camera orientation and camera position
        this->viewMatrix = glm::lookAt(this->cameraPos, this->cameraPos + this->F, this->U);
    }

    //Update function that performs all updates
    void update() {
        this->updateCameraPos();
        this->updateCameraOrientation();
        this->updateViewMatrix();
    }
public:
    //Perform Camera
    void performCamera(GLuint shaderProg) {
        this->update();
        this->render(shaderProg);
    }

    
};

//Create Light Abstract Class
class Light {
protected:
    //Light Fields

    //Light Color
    glm::vec3 lightColor;

    //Ambient strength
    float ambientStr;

    //Ambient Color
    glm::vec3 ambientColor;

    //Spec strength
    float specStr;

    //Spec phong
    float specPhong;

    //Light Brightness
    float brightness;

    //Light Type
    int lightType;

    //Variable address of frag shaders
    GLuint lightColorAddress;
    GLuint ambientStrAddress;
    GLuint ambientColorAddress;
    GLuint cameraPosAddress;
    GLuint specStrAddress;
    GLuint specPhongAddress;
    GLuint brightnessAddress;

public:
    //Constructor
    Light(glm::vec3 lightColor, glm::vec3 ambientColor, float ambientStr,
        float specStr, float specPhong, float brightness) {
        this->lightColor = lightColor;
        this->ambientColor = ambientColor;
        this->ambientStr = ambientStr;
        this->specStr = specStr;
        this->specPhong = specPhong;
        this->brightness = brightness;
    }

    //Pure Virtual Function for children to implement
    virtual void createLight(GLuint shaderProg, glm::vec3 cameraPos) = 0;
};

//Create Point Light from Light
class PointLight :
    public Light {
    //Point Light Fields
private:
    glm::vec3 lightPos; //Position of light

    //Attentuation factor
    float constant;
    float linear;
    float exponent;

    //Neccessary address to frag Shader
    GLuint linearAddress;
    GLuint constantAddress;
    GLuint exponentAddress;
    GLuint lightAddress;

public:
    //Constructor
    PointLight(glm::vec3 lightColor, glm::vec3 ambientColor, float ambientStr,
        float specStr, float specPhong, float brightness,
        glm::vec3 lightPos, float constant, float linear, float exponent) :
        Light(lightColor, ambientColor, ambientStr, specStr, specPhong, brightness) {
        this->lightPos = lightPos;
        this->constant = constant;
        this->linear = linear;
        this->exponent = exponent;
    }

    //Creating the Light Source
    void createLight(GLuint shaderProg, glm::vec3 cameraPos) {


        this->lightAddress = glGetUniformLocation(shaderProg, "lightDirection");
        glUniform3fv(this->lightAddress, 1, glm::value_ptr(this->lightPos));

        this->lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(this->lightColorAddress, 1, glm::value_ptr(this->lightColor));

        this->ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(this->ambientStrAddress, this->ambientStr);

        this->ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(this->ambientColorAddress, 1, glm::value_ptr(this->ambientColor));

        this->cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(this->cameraPosAddress, 1, glm::value_ptr(cameraPos));

        this->specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(this->specStrAddress, this->specStr);

        this->specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(this->specPhongAddress, this->specPhong);

        this->constantAddress = glGetUniformLocation(shaderProg, "constant");
        glUniform1f(this->constantAddress, this->constant);

        this->linearAddress = glGetUniformLocation(shaderProg, "linear");
        glUniform1f(this->linearAddress, this->linear);

        this->exponentAddress = glGetUniformLocation(shaderProg, "exponent");
        glUniform1f(this->exponentAddress, this->exponent);

        this->brightnessAddress = glGetUniformLocation(shaderProg, "brightness");
        glUniform1f(this->brightnessAddress, this->brightness);

    }

    //Getters & Setters
    glm::vec3 getLightPos() {
        return this->lightPos;
    }

    void setLightPos(glm::vec3 lightPos) {
        this->lightPos = lightPos;
    }

    void setIntensity(float linear, float exponent) {
        this->linear = linear;
        this->exponent = exponent;
    }
    void setColor(glm::vec3 lightColor) {
        this->lightColor = lightColor;
    }

};

//Create Directional Light from Light
class DirectionLight :
    public Light {
private:
    //Direction of light
    glm::vec3 lightDirection;

    //light Address to frag Shader
    GLuint lightAddress;
public:
    //Constructor
    DirectionLight(glm::vec3 lightColor, glm::vec3 ambientColor, float ambientStr,
        float specStr, float specPhong, float brightness, glm::vec3 lightDirection) :
        Light(lightColor, ambientColor, ambientStr, specStr, specPhong, brightness) {
        this->lightDirection = lightDirection;
    }

    //Create the light source
    void createLight(GLuint shaderProg, glm::vec3 cameraPos) {

        this->lightAddress = glGetUniformLocation(shaderProg, "lightDirection");
        glUniform3fv(this->lightAddress, 1, glm::value_ptr(this->lightDirection));

        this->lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(this->lightColorAddress, 1, glm::value_ptr(this->lightColor));

        this->ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(this->ambientStrAddress, this->ambientStr);

        this->ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(this->ambientColorAddress, 1, glm::value_ptr(this->ambientColor));

        this->cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(this->cameraPosAddress, 1, glm::value_ptr(cameraPos));

        this->specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(this->specStrAddress, this->specStr);

        this->specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(this->specPhongAddress, this->specPhong);

        this->brightnessAddress = glGetUniformLocation(shaderProg, "brightness");
        glUniform1f(this->brightnessAddress, this->brightness);

    }

    //Setter
    void setIntensity(float brightness) {
        this->brightness = brightness;
    }
    void setColor(glm::vec3 lightColor) {
        this->lightColor = lightColor;
    }
};

int main(void)
{

    //Load the shader file into a string stream
    std::fstream vertSrc("Shaders/Sample.vert");
    std::stringstream vertBuff;

    //Add the file stream to the string stream
    vertBuff << vertSrc.rdbuf();

    //Convert the stream to a character array
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //Load the shader file into a string stream
    std::fstream fragSrc("Shaders/Sample.frag");
    std::stringstream fragBuff;

    //Add the file stream to the string stream
    fragBuff << fragSrc.rdbuf();

    //Convert the stream to a character array
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();


    //Load the shader file into a string stream
    std::fstream skyboxVertSrc("Shaders/Skybox.vert");
    std::stringstream skyboxVertBuff;

    //Add the file stream to the string stream
    skyboxVertBuff << skyboxVertSrc.rdbuf();

    //Convert the stream to a character array
    std::string skyboxVertS = skyboxVertBuff.str();
    const char* sky_v = skyboxVertS.c_str();

    //Load the shader file into a string stream
    std::fstream skyboxFragSrc("Shaders/Skybox.frag");
    std::stringstream skyboxFragBuff;

    //Add the file stream to the string stream
    skyboxFragBuff << skyboxFragSrc.rdbuf();

    //Convert the stream to a character array
    std::string skyboxFragS = skyboxFragBuff.str();
    const char* sky_f = skyboxFragS.c_str();


    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(600, 600, "Mathieu Pobre", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    float window_widith = 600;
    float window_heigth = 600;

    //Create Camera Projections
    //Create Perspective Projection
    MyCamera* cameraFirstPerspective = new PerspectiveCamera(window_heigth, window_widith);
    //Type-cast MyCamera to PerspectiveCamera to use child methods
    PerspectiveCamera* pCamera1Perspective = (PerspectiveCamera*)cameraFirstPerspective;

    //Create Perspective Projection
    MyCamera* cameraThirdPerspective = new PerspectiveCamera(window_heigth, window_widith);
    //Type-cast MyCamera to PerspectiveCamera to use child methods
    PerspectiveCamera* pCamera3Perspective = (PerspectiveCamera*)cameraThirdPerspective;

    //Creaete Orthographic Projection
    MyCamera* cameraOrtho = new OrthoCamera(window_heigth, window_widith);
    OrthoCamera* pCameraOrtho = (OrthoCamera*)cameraOrtho;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    //Mouse and key Listener
    glfwSetKeyCallback(window, Key_Callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    //Create shader after gladLoadGL
    //Shader class
    Shader shader(v, f);


    //Skybox shader
    Shader skyBoxShader(sky_v, sky_f);

    //Instantiate Player
    //Link For Texture and OBJ: https://www.cgtrader.com/free-3d-models/various/various-models/wooden-boat-09a21dad-c4ff-44ba-92e2-1cc2aae336f7
    Player player("3D/boat_Base_Color.png", "3D/boat_Normal.png", "3D/BoatOBJ.obj", shader.getShaderProg());

    //Instantiate Objects
    //Link For Texture and OBJ: https://www.cgtrader.com/free-3d-models/animals/fish/tuna-fish-9f41924a-83d4-478e-a8e9-370946f141b3
    Model3D object1("3D/tuna.png", "3D/tuna_normal.png","3D/TUNAOBJ.obj");

    //Link for Texture and OBJ:https://www.cgtrader.com/free-3d-models/furniture/table/table-model-textures
    Model3D object2("3D/Table_BaseColor.png", "3D/Table_Normal.png", "3D/Table.obj");

    //Link for Texture and OBJ:https://www.cgtrader.com/free-3d-models/furniture/chair/black-leather-chair-d69d98b6-c3c5-44ce-8a67-e2b7a6803d56
    Model3D object3("3D/Chair_BaseColor.png", "3D/Chair_Normal.png", "3D/blackChair.obj");

    //Link for Texture and OBJ:https://www.cgtrader.com/free-3d-models/industrial/other/hard-surface-box-01
    Model3D object4("3D/hardBox.png", "3D/hardbox_normal.png", "3D/Hardbox01.obj");

    //Link for Texture and OBJ:https://www.cgtrader.com/free-3d-models/interior/other/stylized-barrels-441f5f0d-093e-4407-a858-ec92e314ab84
    Model3D object5("3D/Barrel_Base_Color.jpg", "3D/Barrel_Normal_DirectX.jpg", "3D/Barrels.obj");

    //Link for Texture and OBJ:https://www.cgtrader.com/free-3d-models/sports/toy/rubber-duck-b31f3585-0347-4532-bd92-7ddea6107d0d
    Model3D object6("3D/RubberDuck_BaseColor.png", "3D/RubberDuck_Normal.png", "3D/RubberDuck.obj");

    //Set the shaders to the player and object
    object1.setShaders(shader.getShaderProg());
    object2.setShaders(shader.getShaderProg());
    object3.setShaders(shader.getShaderProg());
    object4.setShaders(shader.getShaderProg());
    object5.setShaders(shader.getShaderProg());
    object6.setShaders(shader.getShaderProg());

    //Import the image for the SkyBox
    SkyBox skyBox(skyBoxShader.getShaderProg(), //Skybox shader
        "Skybox/underwater_rt.png", //RIGHT
        "Skybox/underwater_lf.png", //LEFT
        "Skybox/underwater_up.png", //UP
        "Skybox/underwater_dn.png", //DOWN
        "Skybox/underwater_ft.png", //Front
        "Skybox/underwater_bk.png" //Back
    );

    //Create the SKYBOX
    skyBox.createSkyBox();

    //MODEL 1
    object1.createModel();

    //MODEL 2
    object2.createModel();

    //MODEL 3
    object3.createModel();

    //MODEL 4
    object4.createModel();

    //MODEL 5
    object5.createModel();

    //MODEL 6
    object6.createModel();

    //Camera 1
    cameraOrtho->createCamera();

    //Camera 2
    cameraFirstPerspective->createCamera();

    //Camera 3
    cameraThirdPerspective->createCamera();

    //Skybox default Color
    glm::vec3 skyBoxColor = glm::vec3(0,0,0);
    //Direction of light
    glm::vec3 lightDirection = glm::vec3(0, -0.7, 0);

    //Position of Point Light
    glm::vec3 lightPosPoint = glm::vec3(0, -5,0);

    //Light Color
    glm::vec3 lightColor = glm::vec3(1, 1, 1);

    //Ambient strength
    float ambientStr = 0.1f;

    //Ambient Color
    glm::vec3 ambientColor = lightColor;

    //Spec strength
    float specStr = 10.0f;

    //Spec phong
    float specPhong = 50.f;

    //Constant
    float constant = 1.f;
    
    //Linear
    float linear = 0.7f;

    //Exponent
    float exponent = 1.8f;

    //Brightness
    float brightness = 1.f;

    //Instantiate point light and directional light by type-casting light 
    Light* pointLight = new PointLight(lightColor, ambientColor, ambientStr, specStr,
        specPhong, brightness, lightPosPoint, constant, linear, exponent);

    PointLight* pPointLight = (PointLight*)pointLight;

    Light* directionlight = new DirectionLight(lightColor, ambientColor, ambientStr, specStr,
        specPhong, brightness, lightDirection);

    DirectionLight* pDirectionlight = (DirectionLight*)directionlight;

    //Enable blending
    glEnable(GL_BLEND);

    //Choose a blending function
    glBlendFunc(GL_SRC_ALPHA, //Foreground layer
        GL_ONE_MINUS_SRC_ALPHA); //Background Layer

    //Cfinal = Cf(Af) + Cb(1-Af)
    //Cfinal = Cf(Af) - Cb(1-Af) glBelndEquation(GL_FUNC_SUBTRACT);

    //Cfinal = Cf(Af) + Cb(Cf)

    //Andres Rigglsen - Visual glBlendFunc and GlBlendEquation Tool

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDepthMask(GL_FALSE);

        glDepthFunc(GL_LEQUAL);

        //Skybox
        glUseProgram(skyBoxShader.getShaderProg());

        //Use Camera Perspective View and Projection for Skybox
       if (!changeProjection) {
            if (!changePerspective)
                skyBox.generateSkyboxView(pCamera3Perspective->getViewMatrix(), pCamera3Perspective->getProjectionMatrix(), skyBoxColor);
            else
                skyBox.generateSkyboxView(pCamera1Perspective->getViewMatrix(), pCamera1Perspective->getProjectionMatrix(), glm::vec3(0,1,0));
        }
       else {
                skyBox.generateSkyboxView(pCamera3Perspective->getViewMatrix(), pCamera3Perspective->getProjectionMatrix(), skyBoxColor);
        }


        glUseProgram(shader.getShaderProg());

        //Create Light Source
        pPointLight->createLight(shader.getShaderProg(), cameraThirdPerspective->getCameraPos());
        pDirectionlight->createLight(shader.getShaderProg(), cameraThirdPerspective->getCameraPos());
        
        //Cycle Light Intensity
        if (low) {
            pPointLight->setIntensity(linear, exponent);
            pDirectionlight->setIntensity(0.5f);
        }
        if (medium) {
            pPointLight->setIntensity(0.14, 0.07);
            pDirectionlight->setIntensity(1.f);
        }
        if (high) {
            pPointLight->setIntensity(0.0014, 0.000007);
            pDirectionlight->setIntensity(1.5f);
        }
        //Render here

        //Render Model 1
        object1.updateTranslate(-20.f, -10.f, -50.f);
        object1.updateScale(5.f, 5.f, 5.f);
        object1.perform();

        //Render Model 2
        object2.updateTranslate(5.f, -5.f, -10.f);
        object2.updateScale(0.03f, 0.03f, 0.03f);
        object2.perform();

        //Render Model 3
        object3.updateTranslate(0.f, -50.f, -30.f);
        object3.updateScale(2.f, 2.f, 2.f);
        object3.perform();

        //Render Model 4
        object4.updateTranslate(15.f, -80.f, -21.f);
        object4.updateScale(0.05f, 0.05f, 0.05f);
        object4.perform();

        //Render Model 5
        object5.updateTranslate(5.f, -47.f, -25.f);
        object5.updateScale(1.f, 1.f, 1.f);
        object5.perform();

        //Render Model 6
        object6.updateTranslate(-10.f, -100.f, -45.f);
        object6.updateScale(0.05f, 0.05f, 0.05f);
        object6.perform();

        //Change from Perspectivie to Orthographic
        if (!changeProjection) {
            //Change from 3rd to 1st person
            if (!changePerspective) {
                pPointLight->setColor(glm::vec3(1.f, 1.f, 1.f));
                pPointLight->setLightPos(player.getPlayerPos());
                pDirectionlight->setColor(glm::vec3(1, 1, 1));

                pCamera3Perspective->perform(shader.getShaderProg());
                pCamera3Perspective->performCamera(shader.getShaderProg());

                player.setPosition(glm::vec3(0.f + pCamera3Perspective->getCameraPos().x + cos(glm::radians(yaw)) * 10, -5.f + pCamera3Perspective->getCameraPos().y, 0.f + pCamera3Perspective->getCameraPos().z + sin(glm::radians(yaw)) * 10));
                std::cout << "Depth: " << player.getPlayerPos().y << std::endl;
                player.setScale(glm::vec3(1.f, 1.f, 1.f));
                player.setRotation(glm::vec3(0.f, -(-90.f + yaw), 0.f));
                player.perform();
            }
            else {
                pPointLight->setColor(glm::vec3(0.f, 10.f, 0.f));
                pPointLight->setLightPos(player.getPlayerPos());
                pDirectionlight->setColor(glm::vec3(0.f, 10.f, 0.f));

                pCamera1Perspective->setCameraPosition(player.getPlayerPos());
                pCamera1Perspective->updateZfarView();
                pCamera1Perspective->perform(shader.getShaderProg());
                pCamera1Perspective->performCamera(shader.getShaderProg());

                player.setPosition(glm::vec3(0.f + pCamera1Perspective->getCameraPos().x, 0.f + pCamera1Perspective->getCameraPos().y, 0.f + pCamera1Perspective->getCameraPos().z));
                std::cout << "Depth: " << player.getPlayerPos().y << std::endl;
                pCamera3Perspective->setCameraPosition(glm::vec3 (pCamera1Perspective->getCameraPos().x, 5 + player.getPlayerPos().y, pCamera1Perspective->getCameraPos().z + 10));
            }
        }
        else {
            pPointLight->setColor(glm::vec3(1.f, 1.f, 1.f));
            pDirectionlight->setColor(glm::vec3(1.f, 1.f, 1.f));
            //pCameraOrtho->setCameraPosition(player.getPlayerPos());
            pCameraOrtho->perform(shader.getShaderProg());
            pCameraOrtho->performCamera(shader.getShaderProg());
            player.setPosition(glm::vec3(player.getPlayerPos().x,player.getPlayerPos().y,player.getPlayerPos().z));
            player.setRotation(glm::vec3(0.f, 180.f, 0.f));
            player.perform();

        }


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

//Render Texutre Method
void Model3D::renderTexture() {
    glActiveTexture(GL_TEXTURE0);

    this->tex0Address = glGetUniformLocation(this->shaderProg, "tex0");

    glBindTexture(GL_TEXTURE_2D, this->texture);

    glUniform1i(this->tex0Address, 0);

    glActiveTexture(GL_TEXTURE1);

    this->tex1Address = glGetUniformLocation(this->shaderProg, "norm_tex");

    glBindTexture(GL_TEXTURE_2D, this->norm_tex);

    glUniform1i(this->tex1Address, 1);

}