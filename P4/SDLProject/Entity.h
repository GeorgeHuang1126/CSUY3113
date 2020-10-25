#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define COORDS_SIZE 12 

enum  EntityType { PLAYER, ENEMY, PLATFORM };

enum EntityState { STILL, WALKING, DEAD };

enum EntityDir { LEFT, RIGHT };


class Entity {
public:

    EntityType entityType;
    EntityState entityState;
    EntityDir entityDir;
    EntityType lastCollision;
    bool isStatic;
    bool isActive;

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float speed;

    GLuint textures[2];

    glm::vec3 sensorLeft;
    glm::vec3 sensorRight;

    float width;
    float height;

    GLuint textureID;
    float vertices[COORDS_SIZE];
    float texCoords[COORDS_SIZE];

    Entity();

    bool CheckCollision(Entity* other);
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);

    void CheckSensorLeft(Entity *platforms, int platCount);
    void CheckSensorRight(Entity *platforms, int platCount);

    void Update(float deltaTime, Entity *objects, int objectCount);

    void Render(ShaderProgram *program);

    void Jump(float amt);

    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;

    bool sensorLeftCol;
    bool sensorRightCol;

    void startWalk();
    void startJump();
};



