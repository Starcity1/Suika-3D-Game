#define GLM_ENABLE_EXPERIMENTAL

#include "util.h"

#include <sys/stat.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

#include <fstream>
#include <chrono>
#include <thread>
#include "math.h"
#include "shaderSource.h"
#include "shader.h"
#include "Mesh.h"

#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp> // For testing purposes.

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <time.h>
// our stuff
#include "fruits.h"

using namespace std;


#define MAX_BUFFER_SIZE            1024

#define _CAMERA_ROTATE_FACTOR       0.05f

#define _ROTATE_FACTOR              0.005f
#define _SCALE_FACTOR               0.01f
#define _TRANS_FACTOR               0.02f

#define _Z_NEAR                     0.001f
#define _Z_FAR                      100.0f


/***********************************************************************/
/**************************   global variables   ***********************/
/***********************************************************************/

// Window size
unsigned int winWidth  = 800;
unsigned int winHeight = 600;

// Camera
glm::vec3 camera_position = glm::vec3 (0.0f, 0.0f, 3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float camera_angle = 45.0f;
float camera_fovy = 45.0f;
glm::mat4 projection;
float cameraSpeed = 0.01f;
float frameTime = 0;
float prevFrame = 0;
float delta_time = 0.0f; // Time between current frame and last frame
float last_frame = 0.0f; // Time of last frame

// Mouse interaction
bool leftMouseButtonHold = false;
bool isFirstMouse = true;
float prevMouseX;
float prevMouseY;
glm::mat4 modelMatrix = glm::mat4(1.0f);
unordered_map<int, string> keys;

// Vectors to save mesh data PLATFORM
vector<float> render_ver_nor_tex_PLATFORM;       // List of points and normals for rendering
vector<unsigned> render_f_PLATFORM;              // List of faces for rendering

// Vectors to save mesh data SPHERE
vector<float> render_ver_nor_tex_SPHERE;       // List of points and normals for rendering
vector<unsigned> render_f_SPHERE;              // List of faces for rendering

//main data for rendering
Object myObject;
vector<Object> myObjects;

Object platform;

Object sphere;

// render
unsigned int VBO_P, VAO_P, EBO_P;
unsigned int VBO_S, VAO_S, EBO_S;

// declaration
void RotateModel(float angle, glm::vec3 axis);
void TranslateModel(glm::vec3 transVec);
void ScaleModel(float scale);
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);




///=========================================================================================///
///                    Functions to be filled in for Assignment 3    
///          IMPORTANT: you ONLY need to work on these functions in this section      
///=========================================================================================///


void calcPlaneMapping(void)
{
  // Main loop iterates through the entire set of vertices.
  for(auto& vertex : myObject.vertices)
  {
    // Initializing variables for readability.
    float x = vertex.v[0];
    float y = vertex.v[1];
    
    float x_min = myObject.bBox.minP[0];
    float y_min = myObject.bBox.minP[1];
    
    float x_max = myObject.bBox.maxP[0];
    float y_max = myObject.bBox.maxP[1];

    // Now we calculate new values for u and v
    float u = (x - x_min) / (x_max - x_min);
    float v = (y - y_min) / (y_max - y_min);

    // Assigning final values to texture mapping

    vertex.t[0] = u;
    vertex.t[1] = v;
  }
  
}


void calcCylindricalMapping(void)
{ 
  for(auto& vertex : myObject.vertices)
  {
    float x = vertex.v[0];
    float y = vertex.v[1];
    float z = vertex.v[2];
    
    // Calculating angle

    float theta = glm::atan(z, x);

    // Assigning final values to texture mapping

    vertex.t[0] = (PI + theta) / (2 * PI);
    vertex.t[1] = y + 0.5;
  }  
}


void calcSphereMapping(void)
{
   for(auto& vertex : myObject.vertices)
   {
        float x = vertex.v[0];
        float y = vertex.v[1];
        float z = vertex.v[2];

        // Calculating angles
        float rho = glm::sqrt(
            glm::pow(x, 2) +
            glm::pow(y, 2) +
            glm::pow(z, 2)
        );
        float theta = glm::atan(z,x);
        float phi = glm::atan(y, rho);

        // Assigning final values to texture mapping

        vertex.t[0] = (PI + theta) / (2 * PI);
        vertex.t[1] = phi / PI;

   }
}

void calcUVMapping(void)
{
    for(auto& vertex : myObject.vertices)
    {
        float dx = vertex.v[0];
        float dy = vertex.v[1];
        float dz = vertex.v[2];

        float u = 0.5f - glm::atan(dz, dx) / (2 * PI);
        float v = 0.5 + glm::acos(dy) / PI;

        vertex.t[0] = u;
        vertex.t[1] = v;
    }
}

void newGravity(Fruits fruits, float current_frame)
{

    vector<Fruit*> fruitList = fruits.fruits;
    for (int i = 0; i < fruitList.size() - 1; i++) 
    {
        float yPos = fruitList[i]->mat[3][1] + (fruitList[i]->velocity[1] - current_frame * 0.2) * current_frame;
        if (yPos < PLATFORM_BOT + fruitList[i]->radius * RADIUS_SCALE) {
            continue;
        } 
        bool somethingBelow = false;
        for (int j = 0; j < fruitList.size() - 1; j++) {
            if (i != j) {
                Fruit fruit = *fruitList[i];
                glm::vec3 temp = fruitList[j]->mat[3];
                float dist = sqrt(pow(fruit.mat[3][0] - temp[0], 2) + pow(yPos - temp[1], 2) + pow(fruit.mat[3][2] - temp[2], 2));
                if (dist <= fruitList[j]->radius * RADIUS_SCALE + fruit.radius * RADIUS_SCALE) {
                    somethingBelow = true;
                    break;
                }
            }
        }
        if (!somethingBelow) {
            fruitList[i]->velocity[1] -= current_frame * 0.2;
        }
    }
}

///=========================================================================================///
///                            Functions to Load and Render a 3D Model 
///=========================================================================================///

//// scale the model to the same size
void scaleToUnitBox(Object& object)
{
    Point maxP;
    Point minP;

    maxP[0] = maxP[1] = maxP[2] = -1e35;
    minP[0] = minP[1] = minP[2] =  1e35;

    Vector vBoxSize;
    Point  bboxCenterP;

    for (unsigned int i = 0; i < object.vertices.size(); i++)
    {
        Vertex& v = object.vertices[i];

        if (v.v[0] < minP[0])     minP[0] = v.v[0];
        if (v.v[1] < minP[1])     minP[1] = v.v[1];

        if (v.v[2] < minP[2])     minP[2] = v.v[2];
        if (v.v[0] > maxP[0])     maxP[0] = v.v[0];

        if (v.v[1] > maxP[1])     maxP[1] = v.v[1];
        if (v.v[2] > maxP[2])     maxP[2] = v.v[2];
    }

    subPnt(vBoxSize, maxP, minP);

    addScaled(bboxCenterP, minP, vBoxSize, 0.5);

    float modelScale = 1.f / maxComponent3(vBoxSize);

    for (unsigned int i = 0; i < object.vertices.size(); i++)
    {
        Vertex& v = object.vertices[i];

        v.v[0] = (v.v[0] - bboxCenterP[0]) * modelScale;
        v.v[1] = (v.v[1] - bboxCenterP[1]) * modelScale;
        v.v[2] = (v.v[2] - bboxCenterP[2]) * modelScale;
    }

    object.bBox.minP[0] = (minP[0] - bboxCenterP[0]) * modelScale;
    object.bBox.minP[1] = (minP[1] - bboxCenterP[1]) * modelScale;
    object.bBox.minP[2] = (minP[2] - bboxCenterP[2]) * modelScale;

    object.bBox.maxP[0] = (maxP[0] - bboxCenterP[0]) * modelScale;
    object.bBox.maxP[1] = (maxP[1] - bboxCenterP[1]) * modelScale;
    object.bBox.maxP[2] = (maxP[2] - bboxCenterP[2]) * modelScale;

    object.bBox.vSize[0] = vBoxSize[0] * modelScale;
    object.bBox.vSize[1] = vBoxSize[1] * modelScale;
    object.bBox.vSize[2] = vBoxSize[2] * modelScale;
}

// TODO: Modify function to load all objects.
int LoadInput(Object& object, string filename)
{
    /////////////////////////////////////////////////////
    // 1. Open the input file

    // Input file name

    ifstream myfile(filename);

    if (myfile.is_open() == false)
    {
        cout << "Error: cannot open the file.";
        return 1;
    }


    /////////////////////////////////////////////////////
    // 2. Read data from the input file

    vector<glm::vec3> vecv;         // This is the list of points (3D vectors)
    vector<glm::vec3> vecn;         // This is the list of normals (also 3D vectors)
    vector<glm::vec3> vect;         // This is the list of texture coordinates (optional)
    vector<vector<unsigned>> vecf;  // This is the list of faces (indices into vecv and vecn)

    glm::vec3 v;
    string s;
    char buffer[MAX_BUFFER_SIZE];

    // load the OBJ file here
    //writing to vecv, vecn, and vecf

    while ( myfile.getline(buffer, MAX_BUFFER_SIZE) )
    {
        stringstream ss(buffer);

        ss >> s;

        if (s == "v"){
            ss >> v[0] >> v[1] >> v[2];

            vecv.push_back(glm::vec3 (v[0],v[1],v[2]));
        }
        else if (s == "vn"){
            ss >> v[0] >> v[1] >> v[2];

            vecn.push_back(glm::vec3 (v[0],v[1],v[2]));
        }
        else if (s == "vt"){
            ss >> v[0] >> v[1] >> v[2];
            vect.push_back(glm::vec3 (v[0],v[1],v[2]));

        }
        else if (s == "f"){
            string abc;
            string def;
            string ghi;

            vector<unsigned> vu;
            int counter = 0;

            while (counter < 3){
                ss >> abc;

                char charArray1[1024];
                strncpy(charArray1, abc.c_str(), sizeof(charArray1));
                charArray1[sizeof(charArray1) - 1] = 0;

                char *result = strtok(charArray1, "/");
                while (result != NULL){
                    int number = atoi(result);
                    vu.push_back(number);
                    result = strtok (NULL, "/");
                }
                counter ++;
            }

            vecf.push_back(vu);

        }
    }
    myfile.close();

    vector<glm::vec3> vecn_reorder; // same order as the vecv
    vector<glm::vec3> vecf_reorder;
    vecn_reorder.resize(vecv.size());
    vecf_reorder.resize(vecf.size());

    // Re-order the normal vector
    for (int i = 0; i < vecf.size(); ++i)
    {
        // Case 1: input file contains vertex positions, normals, and texture coordinates
        if (vecf[i].size() == 9 )
        {
            vecf_reorder[i][0] = vecf[i][0]-1;
            vecf_reorder[i][1] = vecf[i][3]-1;
            vecf_reorder[i][2] = vecf[i][6]-1;

            vecn_reorder[vecf[i][0]-1] = vecn[vecf[i][2]-1];
            vecn_reorder[vecf[i][3]-1] = vecn[vecf[i][5]-1];
            vecn_reorder[vecf[i][6]-1] = vecn[vecf[i][8]-1];
        }
            // Case 2: input file contains vertex positions and normals
        else if (vecf[i].size() == 6 )
        {
            vecf_reorder[i][0] = vecf[i][0]-1;
            vecf_reorder[i][1] = vecf[i][2]-1;
            vecf_reorder[i][2] = vecf[i][4]-1;

            vecn_reorder[vecf[i][0]-1] = vecn[vecf[i][1]-1];
            vecn_reorder[vecf[i][2]-1] = vecn[vecf[i][3]-1];
            vecn_reorder[vecf[i][4]-1] = vecn[vecf[i][5]-1];
        }
            // Case 3: input file contains vertex positions only
        else if (vecf[i].size() == 3 )
        {
            vecf_reorder[i][0] = vecf[i][0]-1;
            vecf_reorder[i][1] = vecf[i][1]-1;
            vecf_reorder[i][2] = vecf[i][2]-1;
        }
        else
        {
            cout<<"obj format error"<<endl;
        }
    }

    object.vertices.clear();

    for (int i = 0; i < vecv.size(); i++)
    {
        Vertex ver;

        ver.v[0] = vecv[i][0];
        ver.v[1] = vecv[i][1];
        ver.v[2] = vecv[i][2];

        ver.n[0] = vecn_reorder[i][0];
        ver.n[1] = vecn_reorder[i][1];
        ver.n[2] = vecn_reorder[i][2];

        ver.t[0] = 0.0f;
        ver.t[1] = 0.0f;

        object.vertices.push_back( ver );
    }

    Face face;
    object.faces.clear();
    for (int i = 0; i < vecf_reorder.size(); i++)
    {
        face.v1 = vecf_reorder[i][0];
        face.v2 = vecf_reorder[i][1];
        face.v3 = vecf_reorder[i][2];

        object.faces.push_back(face);
    }

    scaleToUnitBox(object);

    return 0;
}

//TODO: Modify function such that ALL objects can be
bool CreateRenderData(Object& object, vector<float>& render_ver, vector<unsigned>& render_f)
{
    if (0 == object.vertices.size())
    {
        return false;
    }
    else
    {

        render_ver.clear();
        render_f.clear();

        for (int i = 0; i < object.vertices.size(); ++i) 
        {

            render_ver.push_back(object.vertices[i].v[0]);
            render_ver.push_back(object.vertices[i].v[1]);
            render_ver.push_back(object.vertices[i].v[2]);

            render_ver.push_back(object.vertices[i].n[0]);
            render_ver.push_back(object.vertices[i].n[1]);
            render_ver.push_back(object.vertices[i].n[2]);

            render_ver.push_back(object.vertices[i].t[0]);
            render_ver.push_back(object.vertices[i].t[1]);
        }

        for (int j = 0; j < object.faces.size(); ++j) 
        {
            render_f.push_back(object.faces[j].v1 );
            render_f.push_back(object.faces[j].v2 );
            render_f.push_back(object.faces[j].v3 );
        }

        return true;
    }
}


///=====================================================///
///         Functions for Moving Camera
///=====================================================///

// void update_camera_position(GLFWwindow* window)
// {
//     float currentFrame = glfwGetTime();
//     delta_time = currentFrame - last_frame;
//     last_frame = currentFrame;

//     float cameraSpeedMultiplier = cameraSpeed * delta_time;

//     if (keys[GLFW_KEY_W]) {
//         camera_position += glm::normalize(camera_up) * cameraSpeedMultiplier;
//         cout << "up!" << endl;
//     }
//     if (keys[GLFW_KEY_S]) {
//         camera_position -= glm::normalize(camera_up) * cameraSpeedMultiplier;
//         cout << "down!" << endl;
//     }
//     if (keys[GLFW_KEY_A]) {
//         camera_position -= glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeedMultiplier;
//         cout << "left!" << endl;
//     }
//     if (keys[GLFW_KEY_D]) {
//         camera_position += glm::normalize(glm::cross(camera_target, camera_up)) * cameraSpeedMultiplier;
//         cout << "right!" << endl;
//     }
// }

///=====================================================///
///         Functions for Manipulating 3D Model  
///=====================================================///

// void activate_gravity(GLFWwindow* window) {
//     // Calculate time change.
//     float current_frame = glfwGetTime();
    
//     // get current position.
//     glm::vec3 gravity_vel_vector = glm::vec3(0.0f, -0.98f, 0.0f) * current_frame * 0.002f;
// }

void RotateModel(float angle, glm::vec3 axis)
{
    glm::vec3 rotateCenter = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

    glm::mat4 rotateMatrix = glm::mat4(1.0f);
    rotateMatrix = glm::translate(rotateMatrix, rotateCenter);
    rotateMatrix = glm::rotate(rotateMatrix, angle, axis);
    rotateMatrix = glm::translate(rotateMatrix, -rotateCenter);

    modelMatrix = rotateMatrix * modelMatrix;
}

void TranslateModel(glm::vec3 transVec)
{
    glm::mat4 translateMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(translateMatrix, transVec);

    modelMatrix = translateMatrix * modelMatrix;
}

void ScaleModel(float scale)
{
    glm::vec3 scaleCenter = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::translate(scaleMatrix, scaleCenter);
    scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
    scaleMatrix = glm::translate(scaleMatrix, -scaleCenter);

    modelMatrix = scaleMatrix * modelMatrix;
}




///=========================================================================================///
///                                    Callback Functions
///=========================================================================================///

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.

    glViewport(0, 0, width, height);

    winWidth  = width;
    winHeight = height;
}


// glfw: whenever a key is pressed, this callback is called
// ----------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Adding key callouts for camera movement.
    glm::vec3 trans = glm::vec3(0.0f);
    if (action == GLFW_PRESS) {
        keys[key] = "PRESS";
    }
    else if (action == GLFW_REPEAT)
    {
        keys[key] = "HOLD";
    } else
    {
        keys[key] = "";
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        leftMouseButtonHold = true;
    }
    else
    {
        leftMouseButtonHold = false;
    }
}


// glfw: whenever the cursor moves, this callback is called
// -------------------------------------------------------
void cursor_pos_callback(GLFWwindow* window, double mouseX, double mouseY)
{
    float  dx, dy;
    float  nx, ny, scale, angle;


    if ( leftMouseButtonHold )
    {
        if (isFirstMouse)
        {
            prevMouseX = mouseX;
            prevMouseY = mouseY;
            isFirstMouse = false;
        }

        else
        {
            float dx =   mouseX - prevMouseX;
            float dy = -(mouseY - prevMouseY); // reversed since y-coordinates go from bottom to top

            prevMouseX = mouseX;
            prevMouseY = mouseY;

            // Rotation
            nx    = -dy;
            ny    =  dx;
            scale = sqrt(nx*nx + ny*ny);

            // We use "ArcBall Rotation" to compute the rotation axis and angle based on the mouse motion
            nx    = nx / scale;
            ny    = ny / scale;
            angle = scale * _ROTATE_FACTOR;

            RotateModel( angle, glm::vec3(nx, ny, 0.0f) );
        }
    }

    else
    {
        isFirstMouse = true;
    }
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    float scale = 1.0f + _SCALE_FACTOR * yOffset;

    ScaleModel( scale );
}


///=========================================================================================///
///                                      RENDER GLFW BINDING
///=========================================================================================///
unsigned int renderstuff(Object& object, vector<float>& render_ver, vector<unsigned>& render_f, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO) 
{
    // create buffers/arrays for surface
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, render_ver.size() * sizeof(float), &render_ver[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, render_f.size() * sizeof(unsigned int), &render_f[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), ((void*)(3* sizeof(float))));
    glEnableVertexAttribArray(1);

    // vertex texture coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // load and create a texture
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture;
}

///=========================================================================================///
///                                      TEXTURE HANDLING
///=========================================================================================///
bool loadTexture(Object& object, vector<float>& render_ver, vector<unsigned>& render_f, shader& myShader, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO)
{
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // Change back to ../data/textures.png
    // "../Blenders/texture_wood.png"
    unsigned char *data = stbi_load((GLASS_TEXTURE), &width, &height, &nrChannels, 0);
    if (data)
    {
        // Adding error handling in case texture is not loaded properly.
        try
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        catch(...)
        {
            perror("Could not load texture");
        }
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // render mesh
    myShader.use();

    // render loop
    // -----------

    // Render projected texture in.
    calcSphereMapping();
    CreateRenderData(object, render_ver, render_f);

    // load data into vertex buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, render_ver.size() * sizeof(float), &render_ver[0], GL_STATIC_DRAW);
    
    return true;
}

///=========================================================================================///
///                                   Draw Fruit Function
///=========================================================================================///

void drawFruit(vector<Fruit*> fruits, glm::vec3 aColor, shader myShader, unsigned int texture_s) 
{
    for (int i = 0; i < fruits.size(); i++)
    {
        glm::mat4 tempMatrix = fruits[i]->mat;

        aColor = glm::vec3 (0.9f, 0.9f, 0.9f);    

        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "model"), 1, GL_FALSE, &tempMatrix[0][0]);
        glUniform3fv(glGetUniformLocation(myShader.ID, "aColor"), 1, &aColor[0]);

        glBindTexture(GL_TEXTURE_2D, texture_s);

        glBindVertexArray(VAO_S);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, render_f_SPHERE.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}


///=========================================================================================///
///                                      Main Function
///=========================================================================================///

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    srand(time(NULL));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int points = 0;

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    // Sets mouse_button_callback() function as handler when mouse press event happens.
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Sets cursor_pos_callback() function as handler when press event happens.
    // glfwSetCursorPosCallback(window, cursor_pos_callback);
    // Sets scroll_callback() function as handler when scroll event happens.
    // glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture the mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // construct the shaders
    //shader
    shader myShader;
    myShader.setUpShader(vertexShaderSource,fragmentShaderSource);

    // PLATFORM
    LoadInput(platform, PLATFORM_PATH);

    CreateRenderData(platform, render_ver_nor_tex_PLATFORM, render_f_PLATFORM);

    unsigned int texture_p = renderstuff(platform, render_ver_nor_tex_PLATFORM, render_f_PLATFORM, VAO_P, VBO_P, EBO_P);
    
    loadTexture(platform, render_ver_nor_tex_PLATFORM, render_f_PLATFORM, myShader, VAO_P, VBO_P, EBO_P);
    
    // SPHERE
    LoadInput(sphere, SPHERE_PATH);

    CreateRenderData(sphere, render_ver_nor_tex_SPHERE, render_f_SPHERE);

    unsigned int texture_s = renderstuff(sphere, render_ver_nor_tex_SPHERE, render_f_SPHERE, VAO_S, VBO_S, EBO_S);
    
    loadTexture(sphere, render_ver_nor_tex_PLATFORM, render_f_PLATFORM, myShader, VAO_S, VBO_S, EBO_S);

    // Update camera's position to a 45deg angle in a unit circle.
    camera_position = (3.0f * glm::vec3(glm::cos(glm::radians(camera_angle)), 1.0f, glm::sin(glm::radians(camera_angle))));

    // Boolean activating gravity.
    bool gravityOn = false;
    
    Fruits* fruits = new Fruits();

    glm::mat4 tempMove = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 1, 0, 1,
    };
    
    
    Blueberry* origin = new Blueberry();
    // fruits->push_fruit(wa1);
    fruits->push_fruit(origin);

    modelMatrix = glm::scale(modelMatrix, glm::vec3(3, 3, 3));    
    while (!glfwWindowShouldClose(window))
    {

        float time = glfwGetTime();
        frameTime = time;
        float current_frame = frameTime - prevFrame;
        prevFrame = time;
        // unsigned int texture = renderstuff(platform, render_ver_nor_tex_PLATFORM, render_f_PLATFORM);
    
        // loadTexture(platform, render_ver_nor_tex_PLATFORM, render_f_PLATFORM, myShader, VAO_P, VBO_P, EBO_P);
        // input
        // -----
        processInput(window);

        // Update camera position.
        // update_camera_position(window);
        const float _CAMERA_MOVE_SPEED = 0.02f;

        for(auto& pair : keys)
        {
            int key = pair.first;
            // Special input to run the scene.
            if (key == GLFW_KEY_SPACE && pair.second == "PRESS")
            {
                int randFruit = rand() % 3; 
                if (randFruit == 0) {
                    fruits->push_fruit(new Blueberry()); 
                }
                else if (randFruit == 1) {
                    fruits->push_fruit(new Cherry());
                }
                else if (randFruit == 2) {
                    fruits->push_fruit(new Lime());
                }
                points += fruits->fruits[fruits->fruits.size() - 2]->radius * 10;
                
                pair.second = "";
                cout << "Points: " << points << endl;
            }

            if (pair.second == "HOLD" || pair.second == "PRESS") {

                if (key == GLFW_KEY_A)
                {
                    // Update camera to go left
                    camera_angle = (camera_angle + _CAMERA_ROTATE_FACTOR);
                    camera_position = (3.0f * glm::vec3(glm::cos(glm::radians(camera_angle)), 1.0f, glm::sin(glm::radians(camera_angle))));
                }
                if (key == GLFW_KEY_D)
                {
                    camera_angle = (camera_angle - _CAMERA_ROTATE_FACTOR);
                    camera_position = (3.0f * glm::vec3(glm::cos(glm::radians(camera_angle)), 1.0f, glm::sin(glm::radians(camera_angle))));
                }
                if (key == GLFW_KEY_W)
                {
                    // Move the camera forward
                    glm::vec3 forward = glm::normalize(glm::vec3(glm::cos(glm::radians(camera_angle)), 0.0f, glm::sin(glm::radians(camera_angle))));
                    camera_position -= _CAMERA_MOVE_SPEED * forward;
                }
                if (key == GLFW_KEY_S)
                {
                    // Move the camera backward
                    glm::vec3 backward = glm::normalize(glm::vec3(glm::cos(glm::radians(camera_angle)), 0.0f, glm::sin(glm::radians(camera_angle))));
                    camera_position += _CAMERA_MOVE_SPEED * backward;
                }
                float ballSpeed = 0.001;
                float extraSpace = 0.05;
                Fruit* curFruit = fruits->fruits[fruits->fruits.size() - 1]; 
                if (key == GLFW_KEY_LEFT)
                {
                    if (curFruit->mat[3][0] > PLATFORM_LEFT + curFruit->radius * RADIUS_SCALE + extraSpace){
                        curFruit->mat[3][0] -= ballSpeed;
                    }
                }
                if (key == GLFW_KEY_RIGHT)
                {
                    if (curFruit->mat[3][0] < PLATFORM_RIGHT - curFruit->radius * RADIUS_SCALE - extraSpace){
                        curFruit->mat[3][0] += ballSpeed;
                    }
                }
                if (key == GLFW_KEY_UP)
                {
                    if (curFruit->mat[3][2] > PLATFORM_UP + curFruit->radius * RADIUS_SCALE + extraSpace){
                        curFruit->mat[3][2] -= ballSpeed;
                    }
                }
                if (key == GLFW_KEY_DOWN)
                {
                    if (curFruit->mat[3][2] < PLATFORM_DOWN - curFruit->radius * RADIUS_SCALE - extraSpace){
                        curFruit->mat[3][2] += ballSpeed;
                    }
                }
            }
        }
        
        fruits->velToMatrixFruits(current_frame * 1.0f, points);


        newGravity(*fruits, current_frame);

        // Clear the buffer
        glClearColor(0.85f, 0.85f, 0.85f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        // view/projection transformations
        projection = glm::perspective(glm::radians(camera_fovy), (float)winWidth / (float)winHeight, _Z_NEAR, _Z_FAR);
        glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);


        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "view"), 1, GL_FALSE, &view[0][0]);

        // render the loaded model
        glm::vec3 aColor = glm::vec3 (0.9f, 0.9f, 0.9f);
        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
        glUniform3fv(glGetUniformLocation(myShader.ID, "aColor"), 1, &aColor[0]);

        // bind Texture
        // PLATFORM
        glBindTexture(GL_TEXTURE_2D, texture_p); 

        glBindVertexArray(VAO_P);

        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, render_f_PLATFORM.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // start of attempting sphere
        
        // texture = renderstuff(sphere, render_ver_nor_tex_SPHERE, render_f_SPHERE);

        // loadTexture(sphere, render_ver_nor_tex_SPHERE, render_f_SPHERE, myShader, VAO_S, VBO_S, EBO_S);
        
        aColor = glm::vec3 (0.9f, 0.9f, 0.9f);        
        drawFruit(fruits->fruits, aColor, myShader, texture_s);
        // glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
        // glUniform3fv(glGetUniformLocation(myShader.ID, "aColor"), 1, &aColor[0]);

        // glBindTexture(GL_TEXTURE_2D, texture_s);

        // glBindVertexArray(VAO_S);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // glDrawElements(GL_TRIANGLES, render_f_SPHERE.size(), GL_UNSIGNED_INT, 0);

        // glBindVertexArray(0);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(myShader.ID);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
