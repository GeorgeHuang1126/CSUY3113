#include "Entity.h"

// construct entity
Entity::Entity()
{
    entityType = PLATFORM;
    isStatic = true;
    isActive = true;
    position = glm::vec3(0);
    speed = 0;
    width = 1;
    height = 0.91;
}

bool Entity::CheckCollision(Entity* other) {
    if (isStatic == true) return false;
    if (isActive == false || other->isActive == false) return false;
    if (other->entityState == DEAD) return false; 

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        lastCollision = other->entityType;
        return true;
    }
    
    return false;
}


void Entity::CheckCollisionsY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity* object = &objects[i];
        
        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2) - (object->height / 2));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                object->collidedBottom = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                object->collidedTop = true;
            }
        }
    }
}


void Entity::CheckCollisionsX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity* object = &objects[i];
        
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2) - (object->width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                object->collidedLeft = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                object->collidedRight = true;
            }
        }
    }
}


bool senseLX = false;
bool senseLY = false;
void Entity::CheckSensorLeft(Entity *platforms, int platCount) {
    
    for (int i = 0; i < platCount; i++) {
        
        if (platforms[i].entityType != PLATFORM) {
            return;
        }
        
        Entity platform  = platforms[i];
        
        if (sensorLeft.x > (platform.position.x-((platform.width) / 2.0f))){
            senseLX = true;
        } else { senseLX = false; }
        
        if (sensorLeft.y < (platform.position.y+((platform.height) / 2.0f))){
            senseLY = true;
        } else { senseLY = false; }
        
        if (senseLX and senseLY and collidedBottom) {
            sensorLeftCol = true;
            return;
        }
    }
    
    sensorLeftCol = false;
}


bool senseRX = false;
bool senseRY = false;
void Entity::CheckSensorRight(Entity *platforms, int platCount) {
    
    for (int i = 0; i < platCount; i++) {
        
        if (platforms[i].entityType != PLATFORM) {
            return;
        }
        
        Entity platform  = platforms[i];
        
        if (sensorRight.x < (platform.position.x+((platform.width) / 2.0f))){
            senseRX = true;
        } else { senseRX = false; }
        
        if (sensorRight.y < (platform.position.y+((platform.height) / 2.0f))){
            senseRY = true;
        } else { senseRY = false; }
        
        if (senseRX and senseRY and collidedBottom) {
            sensorRightCol = true;
            return;
        }
    }
    
    sensorRightCol = false;
}

void Entity::Jump(float amt)
{
    if (collidedBottom) {
        velocity.y = amt;
    }
}


void Entity::startWalk() {
    if (entityType == ENEMY and entityState == WALKING) {
        if (entityDir == LEFT) {
            velocity.x = -1.0f;
        }
        else if (entityDir == RIGHT) {
            velocity.x = 1.0f;
        }
    }
}

void Entity::startJump() {
    if (entityType == ENEMY and entityState == STILL) {
        Jump(2.5f);
    }
}


void Entity::Update(float deltaTime, Entity *objects, int objectCount) {

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    sensorLeftCol = false;
    sensorRightCol = false;
    
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;       
    CheckCollisionsY(objects, objectCount); 
    
    position.x += velocity.x * deltaTime;       
    CheckCollisionsX(objects, objectCount);   
    
    sensorLeft.x = position.x - 0.6f;
    sensorLeft.y = position.y - 0.6f;
    CheckSensorLeft(objects, objectCount);
    
    sensorRight.x = position.x + 0.6f;
    sensorRight.y = position.y - 0.6f;
    CheckSensorRight(objects, objectCount);
    
    for (int i = 0; i < objectCount; i++) {
        
        Entity* other = &objects[i]; 
        
        if ((entityType == PLAYER and other->entityType == ENEMY) and (collidedLeft or collidedRight)) {
            entityState = DEAD;
        }
        
        if ((entityType == PLAYER and other->entityType == ENEMY) and (collidedBottom and other->collidedTop)) {
            other->entityState = DEAD;
        }
    }
}

void Entity::Render(ShaderProgram *program) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
