#ifndef FRUITS_H
#define FRUITS_H

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp> // For testing purposes.

using namespace std;

struct Fruit 
{
    // VARIABLES
    float radius, x, y, z;
    glm::mat4 mat;
    string texture;

    // FUNCTIONS
    Fruit()
    {
        radius = 0;
        x = 0;
        y = 0;
        z = 0;
        mat = glm::mat4(0);
        texture = "";
    }

    Fruit(float radius, float x, float y, float z, string texture, glm::mat4 mat)
    {
        this->radius = radius;
        this->x = x;
        this->y = y;
        this->z = z;
        this->texture = texture;
        this->mat = mat;
    }
    virtual string getTexture(){return texture;}
};

struct Watermelon: Fruit
{
    string getTexture() override {return WMELON_TEXTURE;}

    Watermelon(float radius, float x, float y, float z, string texture, glm::mat4 mat)
    {
        this->radius = radius;
        this->x = x;
        this->y = y;
        this->z = z;
        this->texture = texture;
        this->mat = mat;
    }
};

class Fruits
{
    public:
        vector<Fruit*> fruits;

        // constructor
        Fruits()
        {
            fruits = vector<Fruit*>();
        }

        // push_back function
        void push_fruit(float radius, float x, float y, float z, string texture, glm::mat4 mat) 
        {
            fruits.push_back(new Fruit(radius, x, y, z, texture, mat));
        }
        void push_fruit(Fruit* fruit)
        {
            fruits.push_back(fruit);
        }
};





#endif