/**
 * Util.h
 * 
 * Desc: The following file contains important definitions and some helper functions
 * that are commonly used throughout the project.
*/

#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <glm/glm.hpp>


// Texture paths.
#define PATH ".."
#define SONG_PATH           "../data/スイカゲーム-suika-game-watermelon-game-bgm-made-with-Voicemod.wav"
#define PLATFORM_PATH       "../data/platform.obj"
#define SPHERE_PATH         "../data/sphere.obj"
#define GLASS_TEXTURE       "../data/glass.png"
#define BASE_TEXTURE        "../data/texture_stone.png"
#define APPLE_TEXTURE       "../Blenders/apple.png"
#define BLUEBRRY_TEXTURE    "../Blenders/blueberry.png"
#define BOMB_TEXTURE        "../Blenders/bomb.png"
#define CHERRY_TEXTURE      "../Blenders/cherry.png"
#define LIME_TEXTURE        "../Blenders/lime.png"
#define ORANGE_TEXTURE      "../Blenders/orange.png"
#define COCONUT_TEXTURE     "../Blenders/coconut.png"
#define MELON_TEXTURE       "../Blenders/melon.png"
#define PEACH_TEXTURE       "../Blenders/peach.png"
#define WMELON_TEXTURE      "../Blenders/watermelon.png"

struct ShaderVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    float alpha;
};

// Physicial values
#define GRAVITY_ACCELERATION -.91f

#endif