#include "Level1.h"
#define LEVEL1_WIDTH 25
#define LEVEL1_HEIGHT 8
#define ENEMY_COUNT 2

unsigned int level1_data[] = {
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


void Level1::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");

    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);

    state.player = new Entity();
    state.player->height = 0.8;
    state.player->width = 0.5;
    state.player->position = glm::vec3(2, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
    state.player->jumpPower = 6.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    state.player->entityType = PLAYER;

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTexID = Util::LoadTexture("eevee.png");

    state.enemies[0].textureID = enemyTexID;
    state.enemies[0].position = glm::vec3(17.0f, -4.0f, 0);
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].speed = 1;
    state.enemies[0].width = 0.8;


    state.enemies[1].textureID = enemyTexID;
    state.enemies[1].position = glm::vec3(8.0f, -5.0f, 0);
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].aiType = WAITER;
    state.enemies[1].aiState = IDLE;
    state.enemies[1].speed = 1.5;
    state.enemies[1].width = 0.8;

}

void Level1::Update(float deltaTime) {
    state.player->update(deltaTime, state.map, state.enemies, ENEMY_COUNT, state.player);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].update(deltaTime, state.map, NULL, NULL, state.player);
    }

    if (state.player->position.x >= 24) state.nextScene = 2;

}
void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->render(program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].render(program);
    }
}
