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
    float radius;
    glm::vec3 position;
    glm::mat4 mat;
    string texture;
    glm::vec3 velocity;

    // FUNCTIONS
    // base constructor
    Fruit() 
    {
        radius = 0;
        position = {0, 0, 0};
        mat = glm::scale(glm::mat4(1), glm::vec3(0.2));
        texture = "";
        velocity = {0, 0, 0};
    }

    // default constructor 
    Fruit(float radius, glm::vec3 position, glm::vec3 velocity, string texture, glm::mat4 mat)
    {
        this->radius = radius;
        this->position = position;
        this->texture = texture;
        this->velocity = velocity;
        this->mat = mat;
    }


    virtual string getTexture(){return texture;} // getter function

    void velToMatrix(float current_frame) {
        mat[3] = mat[3] + glm::vec4(velocity[0], velocity[1], velocity[2], 0) * current_frame;
    }
};

struct Watermelon: Fruit
{
    string getTexture() override {return WMELON_TEXTURE;}

    // base constructor
    Watermelon()
    {
        radius = 0;
        position = {0, 0, 0};
        mat = glm::scale(glm::mat4(1), glm::vec3(0.2));
        texture = WMELON_TEXTURE;
        velocity = {0, 0, 0};
    }

    // default constructor
    Watermelon(float radius, glm::vec3 position, glm::vec3 velocity, string texture, glm::mat4 mat)
    {
        this->radius = radius;
        this->position = position;
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
        void push_fruit(float radius, glm::vec3 position, glm::vec3 velocity, string texture, glm::mat4 mat) 
        {
            fruits.push_back(new Fruit(radius, position, velocity, texture, mat));
        }
        void push_fruit(Fruit* fruit)
        {
            fruits.push_back(fruit);
        }

        // Mat Manipulator
        void velToMatrixFruits(float current_frame) {
            for (int i = 0; i < fruits.size(); i++) {
                fruits[i]->velToMatrix(current_frame);
            }
        }
};





#endif