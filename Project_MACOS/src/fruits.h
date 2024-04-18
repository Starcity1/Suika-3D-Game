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
struct Fruit;
void merge(vector<Fruit*>& fruits, int i, int curI, int& points);

// Generating small Texture structure
struct Texture
{
    string texture_path;
    unsigned int id;
    int width;
    int height;
    int rChannels;

    Texture() : texture_path(BASE_TEXTURE) {}
};

struct Fruit 
{
    // VARIABLES
    // radius is equal to the float inside the scale function
    float radius;
    // position is the value of the first 3 elements in the mat[3] matrix
    glm::vec3 position;
    // model matrix
    glm::mat4 mat;
    Texture texture;
    // every frame our balls move by this velocity
    glm::vec3 velocity;

    bool merged;
    unsigned int VAO, VBO, EBO;

    bool beingErased = false;

    // FUNCTIONS
    // base constructor
    Fruit()
    {
        radius = 0;
        position = {0, 0, 0};
        mat = glm::scale(glm::mat4(1), glm::vec3(0.2));
        texture.texture_path = "";
        merged = false;
        velocity = {0, 0, 0};
    }

    // default constructor 
    Fruit(float radius, glm::vec3 position, glm::vec3 velocity, string texture, glm::mat4 mat)
    {
        this->radius = radius;
        this->position = position;
        this->texture.texture_path = texture;
        this->velocity = velocity;
        this->mat = mat;
    }


    virtual string getTexture(){return texture.texture_path;} // getter function

       bool GJK(vector<Fruit*>& fruits, Fruit& fruit, glm::vec4& temp, int i, int curI, int& points) {
        float distToReal = sqrt(pow(fruit.mat[3][0] - mat[3][0], 2) + pow(fruit.mat[3][1] - mat[3][1], 2) + pow(fruit.mat[3][2] - mat[3][2], 2));
        if (distToReal <= radius * RADIUS_SCALE + fruit.radius * RADIUS_SCALE) {
            cout << "distToReal: " << distToReal << endl;
            cout << "radius * RADIUS_SCALE + fruit.radius * RADIUS_SCALE" << radius * RADIUS_SCALE + fruit.radius * RADIUS_SCALE << endl;
            glm::vec4 dir = glm::vec4(fruit.mat[3][0] - mat[3][0], fruit.mat[3][1] - mat[3][1], fruit.mat[3][2] - mat[3][2], 0);
            mat[3] = mat[3] - dir * 0.001f;
            fruit.mat[3] = fruit.mat[3] + dir * 0.001f;
            return true;
        }


        float dist = sqrt(pow(fruit.mat[3][0] - temp[0], 2) + pow(fruit.mat[3][1] - temp[1], 2) + pow(fruit.mat[3][2] - temp[2], 2));
        if (dist <= radius * RADIUS_SCALE + fruit.radius * RADIUS_SCALE) {
            if (fruit.radius == this->radius && !fruit.beingErased && !beingErased) {
                beingErased = true;
                fruit.beingErased = true;
                merge(fruits, i, curI, points);
            }
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
            velocity[0] = velocity[0] + (v1n_new - v1n) * nx * 0.9;
            velocity[1] = velocity[1] + (v1n_new - v1n) * ny * 0.9;
            velocity[2] = velocity[2] + (v1n_new - v1n) * nz * 0.9;

            fruit.velocity[0] = fruit.velocity[0] + (v2n_new - v2n) * nx * 0.9;
            fruit.velocity[1] = fruit.velocity[1] + (v2n_new - v2n) * ny * 0.9;
            fruit.velocity[2] = fruit.velocity[2] + (v2n_new - v2n) * nz * 0.9;
            return true;
        }

        return false;
    }
    // handles movement
    void velToMatrix(float current_frame, vector<Fruit*>& fruits, int curI, int& points) {
        float threshold = 0.1;
        glm::vec4 temp = mat[3] + glm::vec4(velocity[0], velocity[1], velocity[2], 0) * current_frame;
        float velModifier = -0.5;
        bool didTouch = false;
        for (int i = 0; i < fruits.size() - 1; i++) {
            if (i != curI) {
                if (this->GJK(fruits, *fruits[i], temp, i, curI, points)) {
                    didTouch = true;
                }
            }
        }
        beingErased = false;
        if (didTouch) {
            return;
        }
        if (temp[1] >= PLATFORM_BOT + radius * RADIUS_SCALE) {
            mat[3][1] = temp[1];
        } else {
            velocity = velocity * 0.80f;
            velocity[1] = -0.01f * velocity[1];
            mat[3][1] = PLATFORM_BOT + radius * RADIUS_SCALE;
        }
        if (temp[0] >= PLATFORM_LEFT + radius * RADIUS_SCALE && temp[0] <= PLATFORM_RIGHT - radius * RADIUS_SCALE) {
            mat[3][0] = temp[0];
        } else {
            velocity[0] = velModifier * velocity[0];
            if (temp[0] < PLATFORM_LEFT + radius * RADIUS_SCALE) {
                mat[3][1] += (PLATFORM_LEFT + radius * RADIUS_SCALE) - temp[0];
                mat[3][0] = PLATFORM_LEFT + radius * RADIUS_SCALE;
            } 
            if (temp[0] > PLATFORM_RIGHT - radius * RADIUS_SCALE) {
                mat[3][1] += temp[0] - (PLATFORM_RIGHT - radius * RADIUS_SCALE);
                mat[3][0] = PLATFORM_RIGHT - radius * RADIUS_SCALE;
            }
        }
        if (temp[2] >= PLATFORM_UP + radius * RADIUS_SCALE && temp[2] <= PLATFORM_DOWN - radius * RADIUS_SCALE) {
            mat[3][2] = temp[2];
        } else {
            velocity[2] = velModifier * velocity[2];

            if (temp[2] < PLATFORM_UP + radius * RADIUS_SCALE) {
                mat[3][1] += (PLATFORM_UP + radius * RADIUS_SCALE) - temp[2];
                mat[3][2] = PLATFORM_UP + radius * RADIUS_SCALE;
            }
            if (temp[2] > PLATFORM_DOWN - radius * RADIUS_SCALE) {
                mat[3][1] += temp[2] - (PLATFORM_DOWN - radius * RADIUS_SCALE);
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
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 1.4;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = WMELON_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Watermelon(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 1.4;
        this->texture.texture_path = WMELON_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
    }
};

struct Coconut: Fruit
{
    string getTexture() override {return COCONUT_TEXTURE;}

    // base constructor
    Coconut()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 1.4;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = COCONUT_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Coconut(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 1.4;
        this->texture.texture_path = COCONUT_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
    }
};

struct Peach: Fruit
{
    string getTexture() override {return PEACH_TEXTURE;}

    // base constructor
    Peach()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 1.4;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = PEACH_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Peach(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 1.4;
        this->texture.texture_path = PEACH_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
    }
};

struct Apple: Fruit
{
    string getTexture() override {return APPLE_TEXTURE;}

    // base constructor
    Apple()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 1.2;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = APPLE_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Apple(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 1.2;
        this->texture.texture_path = APPLE_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
    }
};

struct Orange: Fruit
{
    string getTexture() override {return BOMB_TEXTURE;}

    // base constructor
    Orange()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 1.0;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = BOMB_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Orange(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 1.0;
        this->texture.texture_path = BOMB_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
    }
};

struct Lime: Fruit
{
    string getTexture() override {return MELON_TEXTURE;}

    // base constructor
    Lime()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 0.8;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = MELON_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Lime(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 0.8;
        this->texture.texture_path = MELON_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
    }
};

struct Cherry: Fruit
{
    string getTexture() override {return WMELON_TEXTURE;}

    // base constructor
    Cherry()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 0.6;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = WMELON_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Cherry(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 0.6;
        this->texture.texture_path = WMELON_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
    }
};

struct Blueberry: Fruit
{
    string getTexture() override {return BLUEBRRY_TEXTURE;}

    // base constructor
    Blueberry()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 0.6;
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = BLUEBRRY_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Blueberry(glm::vec3 velocity, glm::mat4 mat)
    {
        this->radius = 0.6;
        this->texture.texture_path = BLUEBRRY_TEXTURE;
        this->mat = mat;
        this->velocity = velocity;
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
        void velToMatrixFruits(float current_frame, int& points) {
            for (int i = 0; i < fruits.size() - 1; i++) {
                fruits[i]->velToMatrix(current_frame, fruits, i, points);
            }
        }
};

void merge(vector<Fruit*>& fruits, int i, int curI, int& points){
    // handle fruit deletions
    glm::vec4 pos = fruits[curI]->mat[3];
    float radius = fruits[curI]->radius;
    delete fruits[curI];

    if (radius == 0.4f) { // GENERATE CHERRY

        fruits[curI] = new Cherry();
        
    }
    else if (radius == 0.6f) { // GENERATE LIME

        fruits[curI] = new Lime();
        
    }
    else if (radius == 0.8f) { // GENERATE ORANGE

        fruits[curI] = new Orange();
        
    }
    else if (radius == 1.0f) { // GENERATE APPLE

        fruits[curI] = new Apple();
        
    }
    else if (radius == 1.2f) { // GENERATE PEACH

        fruits[curI] = new Peach();
        
    }
    else if (radius == 1.4f) { // GENERATE COCONUTS

        fruits[curI] = new Coconut();
        
    }
    else if (radius == 1.6f) { // GENERATE WATERMELON

        fruits[curI] = new Watermelon();
        
    }
    fruits[curI]->merged = true;
    fruits[curI]->mat[3] = pos;
    points += radius * 10;
    cout << "Points: " << points << endl;
    fruits.erase(fruits.begin() + i);
}

#endif