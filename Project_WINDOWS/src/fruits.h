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

#define PLATFORM_BOT -1
#define PLATFORM_LEFT -1.45
#define PLATFORM_RIGHT 1.45
#define PLATFORM_UP -1.45
#define PLATFORM_DOWN 1.45
#define RADIUS_SCALE 0.5

using namespace std;

struct Fruit 
{
    // VARIABLES
    // radius is equal to the float inside the scale function
    float radius;
    // position is the value of the first 3 elements in the mat[3] matrix
    glm::vec3 position;
    // model matrix
    glm::mat4 mat;
    string texture;
    // every frame our balls move by this velocity
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

    bool GJK(Fruit& fruit, glm::vec4& temp) {
        float dist = sqrt(pow(fruit.mat[3][0] - temp[0], 2) + pow(fruit.mat[3][1] - temp[1], 2) + pow(fruit.mat[3][2] - temp[2], 2));
        if (dist <= radius * RADIUS_SCALE + fruit.radius * RADIUS_SCALE) {
            // glm::vec4 dir = glm::vec4(fruit.mat[3][0] - mat[3][0], fruit.mat[3][1] - mat[3][1], fruit.mat[3][2] - mat[3][2], 0);
            // float velM = 0.9;
            // float tempX = velocity[0];

            // velocity[0] = fruit.velocity[0] * velM; 
            // fruit.velocity[0] = tempX * velM;

            // float tempZ = velocity[2];

            // velocity[2] = fruit.velocity[2] * velM; 
            // fruit.velocity[2] = tempZ * velM;

            // float tempY = velocity[1];

            // velocity[1] = fruit.velocity[1];
            // fruit.velocity[1] = tempY;

            float nx = (fruit.mat[3][0] - mat[3][0]) / dist;
            float ny = (fruit.mat[3][1] - mat[3][1]) / dist;
            float nz = (fruit.mat[3][2] - mat[3][2]) / dist;

            // Normal components of velocity
            float v1n = velocity[0] * nx + velocity[1] * ny + velocity[2] * nz;
            float v2n = fruit.velocity[0] * nx + fruit.velocity[1] * ny + fruit.velocity[2] * nz;

            // Updated normal velocities after collision
            float v1n_new = (v1n * (radius - fruit.radius) + 2 * fruit.radius * v2n) / (radius + fruit.radius);
            float v2n_new = (v2n * (fruit.radius - radius) + 2 * radius * v1n) / (radius + fruit.radius);

            // Update velocities
            velocity[0] = velocity[0] + (v1n_new - v1n) * nx;
            velocity[1] = velocity[1] + (v1n_new - v1n) * ny * 0.9;
            velocity[2] = velocity[2] + (v1n_new - v1n) * nz;

            fruit.velocity[0] = fruit.velocity[0] + (v2n_new - v2n) * nx;
            fruit.velocity[1] = fruit.velocity[1] + (v2n_new - v2n) * ny * 0.9;
            fruit.velocity[2] = fruit.velocity[2] + (v2n_new - v2n) * nz;
            return true;
        }

        return false;
    }
    // handles movement
    void velToMatrix(float current_frame, vector<Fruit*>& fruits, int curI) {
        glm::vec4 temp = mat[3] + glm::vec4(velocity[0], velocity[1], velocity[2], 0) * current_frame;

        float velModifier = -0.5;
        bool didTouch = false;
        for (int i = 0; i < fruits.size() - 1; i++) {
            if (i != curI) {
                if (this->GJK(*fruits[i], temp)) {
                    didTouch = true;
                }
            }
        }
        if (didTouch) {
            return;
        }
        if (temp[1] >= PLATFORM_BOT + radius * RADIUS_SCALE) {
            mat[3][1] = temp[1];
        } else {
            velocity = velocity * 0.999f;
            velocity[1] = -0.01f * velocity[1];
            mat[3][1] = PLATFORM_BOT + radius * RADIUS_SCALE;
        }
        if (temp[0] >= PLATFORM_LEFT + radius * RADIUS_SCALE && temp[0] <= PLATFORM_RIGHT - radius * RADIUS_SCALE) {
            mat[3][0] = temp[0];
        } else {
            velocity[0] = velModifier * velocity[0];
            if (temp[0] < PLATFORM_LEFT + radius * RADIUS_SCALE) {
                mat[3][0] = PLATFORM_LEFT + radius * RADIUS_SCALE;
            } 
            if (temp[0] > PLATFORM_RIGHT - radius * RADIUS_SCALE) {
                mat[3][0] = PLATFORM_RIGHT - radius * RADIUS_SCALE;
            }
        }
        if (temp[2] >= PLATFORM_UP + radius * RADIUS_SCALE && temp[2] <= PLATFORM_DOWN - radius * RADIUS_SCALE) {
            mat[3][2] = temp[2];
        } else {
            velocity[2] = velModifier * velocity[2];

            if (temp[2] < PLATFORM_UP + radius * RADIUS_SCALE) {
                mat[3][2] = PLATFORM_UP + radius * RADIUS_SCALE;
            }
            if (temp[2] > PLATFORM_DOWN - radius * RADIUS_SCALE) {
                mat[3][2] = PLATFORM_DOWN - radius * RADIUS_SCALE;
            }
        }
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
            for (int i = 0; i < fruits.size() - 1; i++) {
                fruits[i]->velToMatrix(current_frame, fruits, i);
            }
        }
};





#endif