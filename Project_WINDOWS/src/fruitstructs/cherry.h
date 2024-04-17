#ifndef CHERRY_H
#define CHERRY_H

#include "../fruits.h"


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
        position = {0, 0, 0};
        mat = glm::scale(tempMove, glm::vec3(radius));
        texture = WMELON_TEXTURE;
        velocity = {0, -1, 0};
    }

    // default constructor
    Cherry(float radius, glm::vec3 position, glm::vec3 velocity, string texture, glm::mat4 mat)
    {
        this->radius = radius;
        this->position = position;
        this->texture = texture;
        this->mat = mat;
    }
};

#endif