#ifndef BLUEBERRY_H
#define BLUEBERRY_H

#include "../fruits.h"



struct Blueberry: Fruit
{
    string getTexture() override {return WMELON_TEXTURE;}

    // base constructor
    Blueberry()
    {
        glm::mat4 tempMove = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1, 0, 1,
        };
        radius = 0.4;
        position = {0, 0, 0};
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture.texture_path = WMELON_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Blueberry(float radius, glm::vec3 position, glm::vec3 velocity, string texture, glm::mat4 mat)
    {
        this->radius = radius;
        this->position = position;
        this->texture.texture_path = texture.c_str();
        this->mat = mat;
    }
};

#endif